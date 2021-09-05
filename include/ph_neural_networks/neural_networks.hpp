#pragma once
#include <limits>
#include <numbers>
#include <iostream>
#include <ph_concepts/concepts.hpp>
#include <latch>
#include <poll.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>

using std::cout, std::endl;

using namespace ph::concepts;



namespace ph::network
{
    template <typename T>
    auto operator+= (std::vector <T>& v, auto&& e)
    {
        v.push_back (std::forward <decltype (e)> (e));
        return v;
    }
    
    struct types {};
    
    
    auto host_to_nework (short i/*htons*/)
    {
        return htons (i);
    }
    
    auto host_to_nework (long i/*htonl*/)
    {
        return htonl (i);
    }
    
    
    
    
    auto network_to_host (short i/*ntohs*/)
    {
        return ntohs (i);
    }
    
    
    auto network_to_host (long i/*ntohl*/)
    {
        return ntohl (i);
    }
    
    
    
    
    //    template <typename...>
    //    struct socket;
    //
    //    template <>
    //    struct socket <types>
    //    {
    //
    //
    //        constexpr auto begin () noexcept
    //        {
    ////            return
    //        }
    //    };
    
    
    
    template <typename T>
    concept Address = String <T>;
    
    template <typename T>
    concept Socket = std::is_convertible_v <T, int>;
    
    template <typename T>
    concept Port = std::is_convertible_v <T, unsigned short>;
    
    template <typename T>
    concept Server = requires ()
    {
        true;
    };
}


auto poll_server ()
{
    struct pollfd pfds[1]; // More if you want to monitor more
    pfds[0].fd = 0; // Standard input
    pfds[0].events = POLLIN; // Tell me when ready to read
    
    int num_events = poll (pfds, 1, 2500); // 2.5 second timeout
    
    std::cout << "tjo" << std::endl;
    if (num_events == 0) { printf("Poll timed out!\n");
    } else {
        int pollin_happened = pfds[0].revents & POLLIN;
        if (pollin_happened) {
            printf("File descriptor %d is ready to read\n", pfds[0].fd);
        } else {
            printf("Unexpected event occurred: %d\n", pfds[0].revents);
        } }
}

uint64_t pack754(long double f, unsigned bits, unsigned expbits)
{
    long double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit
    
    if (f == 0.0) return 0; // get this special case out of the way 14
                            // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }
    
    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;
    
    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);
    
    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}


long double unpack754(uint64_t i, unsigned bits, unsigned expbits)
{
    long double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit
    
    if (i == 0) return 0.0;
    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on
    
    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }
    
    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;
    
    return result;
}



#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))



namespace ph::process
{
    auto id () -> pid_t
    {
        return getpid ();
    }
    
    auto parent_id () -> pid_t
    {
        return getppid ();
    }
    
    auto fork () -> pid_t
    {
        return fork ();
    }
}

#include <sys/msg.h>
namespace ph
{
    struct msg_queue
    {
        key_t key;
        int queue_id;
        
        msg_queue (auto path) : key {ftok (path, 'b')}, queue_id {msgget (key, 0666 | IPC_CREAT)}
        {
            
        }
        
        ~msg_queue ()
        {
            msgctl (queue_id, IPC_RMID, NULL);
        }
        
        auto send (auto value)
        requires requires
        {
            std::is_same_v <decltype (value.mtype), long>;
        }
        {
            msgsnd (queue_id, &value, sizeof (decltype (value)), 0);
        }
        
        auto receive (auto& value)
        requires requires
        {
            std::is_same_v <decltype (value.mtype), long>;
        }
        {
            msgrcv (queue_id, &value, sizeof (decltype (value)), 2, 0);
        }
    };
    auto create_key (auto path)
    {
        return ftok (path, 'b');
    }
    
    auto create_id ()
    {
        
//        return msgget (key, 0666 | IPC_CREAT);
    }
    
}


namespace ph
{
    template <typename T>
    struct shared_memory
    {
        key_t key;
        int id;
        T* data;
        
        shared_memory (auto path, auto bytes) : key {ftok (path, 'R')}, id {shmget (key, bytes, 0644 | IPC_CREAT)}, data {static_cast <T*> (shmat (id, (void *)0, 0))}
        {
            
            if (data == (T *)(-1))
                perror ("shmat");
        }
        
        ~shared_memory ()
        {
            shmdt (data);
            shmctl (id, IPC_RMID, NULL);
        }
    };
}
