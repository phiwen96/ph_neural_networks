
#define RUN_CATCH

#if defined (RUN_CATCH)

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>




auto main (int argc, char* argv[]) -> int
{
    int result = Catch::Session().run( argc, argv );
    return result;
}

#else

#include <ph_network/network.hpp>
#include <thread>
#include <filesystem>
#include <string>
#include <ph_network/common.hpp>
#include <chrono>

auto get_in_addr = [] (sockaddr* sa) -> void*
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    else
        return &(((sockaddr_in6*)sa)->sin6_addr);
};

auto server = [](char const* address = nullptr, char const* port = "8080")
{
    addrinfo* servinfo;
    int s;
    
    auto hints = addrinfo
    {
        
    };

    hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
    
    if (auto err = getaddrinfo (nullptr, port, &hints, &servinfo); err != 0)
    {
        
        fprintf (stderr, "getaddrinfo error: %s\n", gai_strerror (err));
        exit (1);
    }
    
    auto* p = servinfo;
    
    for (; p != nullptr; p = p -> ai_next)
    {
        char ipstr [INET6_ADDRSTRLEN];
        [&]()
        {
            void *addr;
            auto ipver = "IPv4";;
            
            if (p->ai_family == AF_INET) { // IPv4
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr; addr = &(ipv4->sin_addr);
                
            } else { // IPv6
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr; addr = &(ipv6->sin6_addr);
                ipver = "IPv6";
            }
            inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
            std::cout << ipver << "=" << ipstr << std::endl;
            //            std::cout << "::" << ipstr << std::endl;
        }();
        
        void *addr;
        char *ipver;
        
        if ((s = socket (p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror ("server: socket");
            continue;
        }
        auto yes = 1 ;
        if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1)
        {
            perror ("setsockopt");
            exit (1);
        }
        if (bind (s, p->ai_addr, p->ai_addrlen) == -1)
        {
            close (s);
            perror ("server: bind");
            continue;
        }
        
        break;
    }
    
    freeaddrinfo (servinfo);
    
    if (p == nullptr)
    {
        std::cout << "Server failed to bind." << std::endl;
        exit (1);
    }
    
    
    fcntl (s, F_SETFL, O_NONBLOCK); // set to non-blocking
    fcntl (s, F_SETFL, O_ASYNC); // set to asynchronous I/O
    
    if (listen (s, 10) == -1)
    {
        throw;
    }
    
    std::cout << "server: waiting for connections..." << std::endl;
    
    while (true)
    {
        auto their_addr = sockaddr_storage {};
        auto addr_size = socklen_t {sizeof (their_addr)};
        //    std::cout << "kuk" << std::endl;
        auto client = accept (s, (struct sockaddr*) &their_addr, &addr_size);
        
        if (client < 0)
        {
            perror ("Unable to accept");
            throw;
        }
        
        /**/
        
        
        
        char hostname [INET6_ADDRSTRLEN];
        
        inet_ntop (their_addr.ss_family, get_in_addr ((struct sockaddr *)&their_addr), hostname, sizeof hostname);
        
        std::cout << "server::connection(host = " << hostname << ")" << std::endl;
        
        char buf [1024];
        
        if (read (client, buf, 1024) < 0)
        {
            throw;
        }
        std::cout << "server::got_message(\"" << buf << "\")" << std::endl;
        //    std::cout << buf << std::endl;
        
        //        auto msg = "Hello from server.";
        auto header = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
        
        write (client, header, strlen (header));
        
        //        auto* ssl = SSL_new(ssl_ctx);
        //                SSL_set_fd(ssl, client);
        //
        //                if (SSL_accept(ssl) <= 0) {
        //                    ERR_print_errors_fp(stderr);
        //                }
        //                else {
        //                    SSL_write(ssl, header, strlen (header));
        //                }
        //
        //                SSL_shutdown(ssl);
        //                SSL_free(ssl);
    }
    
    
    
    //    close (s);
    
    
    
    //    auto addr_size = sizeof their_addr;
    
};

auto client = [](char const* address = nullptr, char const* port = "8080")
{
    addrinfo* res;
    
    auto hints = addrinfo
    {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        //        .ai_flags = AI_PASSIVE
    };
    
    if (auto err = getaddrinfo (address, port, &hints, &res); err != 0)
    {
        
        fprintf (stderr, "getaddrinfo error: %s\n", gai_strerror (err));
        exit (1);
    }
    auto s = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
    
    if (s == -1)
    {
        perror ("socket");
        exit (1);
    }
    
    auto err = connect (s, res->ai_addr, res->ai_addrlen);
    
    //    std::cout << "kuk" << std::endl;
    if (err == -1)
    {
        //        fprintf (stderr, "getaddrinfo error: %s\n", gai_strerror (err));
        throw;
        //        exit(1);
    }
    //    std::cout << "kuk" << std::endl;
    
    auto msg = "Hello from client!";
    
    send (s, msg, strlen (msg), 0);
    
    close (s);
    
    //    std::byte buf [1024];
    //
    //    if (read (s, buf, 1024) < 0)
    //    {
    //        throw;
    //    }
    
    
    //    return;
    
    freeaddrinfo (res);
    
    //    std::cout << "yo" << std::endl;
    
};


auto init_ssl ()
{
    SSL_CTX *ctx;
    
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    
    const SSL_METHOD *method;
    
    method = SSLv23_server_method();
    
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    SSL_CTX_set_ecdh_auto(ctx, 1);
    
    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}



auto main (int argc, char* argv[]) -> int
{
    ph::network::test_0 ();
    return 0;
    //    [](char const* ip_address, char const* port){
    //        addrinfo* servinfo;
    //        int s;
    //
    //        auto hints = addrinfo
    //        {
    //            .ai_family = AF_UNSPEC,
    //            .ai_socktype = SOCK_STREAM,
    //            .ai_flags = AI_PASSIVE
    //        };
    //
    //        if (auto err = getaddrinfo (nullptr, ip_address, &hints, &servinfo); err != 0)
    //        {
    //
    //            fprintf (stderr, "getaddrinfo error: %s\n", gai_strerror (err));
    //            exit (1);
    //        }
    //
    //        auto* p = servinfo;
    //
    //        char ipstr [INET6_ADDRSTRLEN];
    //
    //        for (; p != nullptr; p = p -> ai_next)
    //        {
    //            [&]()
    //            {
    //                void *addr;
    //                char *ipver;
    //
    //                if (p->ai_family == AF_INET) { // IPv4
    //                struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr; addr = &(ipv4->sin_addr);
    //                ipver = "IPv4";
    //                } else { // IPv6
    //                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr; addr = &(ipv6->sin6_addr);
    //                ipver = "IPv6";
    //                }
    //                inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    //                std::cout << ipver << "=" << ipstr << std::endl;
    //    //          q   std::cout << "::" << ipstr << std::endl;
    //            }();
    //        }
    //    }(nullptr, "8080");
        server (nullptr, /*port*/"8080");
//     []{
//         int sock = socket (PF_INET, SOCK_DGRAM, 0);
        
//         auto loopback = sockaddr_in
//         {
//             .sin_family = AF_INET,
//             .sin_addr.s_addr = 1337,   // can be any IP address
//             .sin_port = htons(9)      // using debug port
//         };
        
//         if (sock == -1) {
//             std::cerr << "Could not socket\n";
//             return 1;
//         }
        
// //        std::memset(&loopback, 0, sizeof(loopback));
        
        
//         if (connect(sock, reinterpret_cast<sockaddr*>(&loopback), sizeof(loopback)) == -1) {
//             close(sock);
//             std::cerr << "Could not connect\n";
//             return 1;
//         }
        
//         socklen_t addrlen = sizeof(loopback);
//         if (getsockname(sock, reinterpret_cast<sockaddr*>(&loopback), &addrlen) == -1) {
//             close(sock);
//             std::cerr << "Could not getsockname\n";
//             return 1;
//         }
        
//         close(sock);
        
//         char buf[INET_ADDRSTRLEN];
//         if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == 0x0) {
//             std::cerr << "Could not inet_ntop\n";
//             return 1;
//         } else {
//             std::cout << "Local ip address: " << buf << "\n";
//         }
//     };
    return 0;
    
    //    const auto p1 = std::chrono::system_clock::now();
    //
    //        std::cout << "seconds since epoch: "
    //                  << std::chrono::duration_cast <std::chrono::seconds> (std::chrono::system_clock::now().time_since_epoch()).count() << '\n';
    //
    //
    //    using namespace std::chrono;
    //    int64_t timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    //
    //    std::cout << timestamp << std::endl;
    //    return 0;
    //    if(argc < 2){
    //        std::cout<<"./httpserver <resource-directory-path>"<<std::endl;
    //        return 0;
    //      }
    
    //    std::string path (argv[1]);
    
    //     if(path.at(path.length() - 1) == '/')
    //       path.pop_back();
    //
    //     auto str = std::move(path);
    //    std::cout << str << std::endl;
    
    //    auto th1 = std::thread{[](){
    //        server (/*port*/"8080");
    //    }};
    //    auto th2 = std::thread{[](){
    //
    //        for (int i = 0; i < 10; ++i)
    //        {
    //            client (/*address*/"localhost", /*port*/"8080");
    //        }
    //        std::cout << "client" << std::endl;
    //    }};
    
    //	auto f = std::ifstream ("testlib.cpp");
    //    auto f = readFile ("testlib.cpp");
    
    //    return;
    
    //    th1.join ();
    //    th2.join ();
    
    
}


#endif

