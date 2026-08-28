#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#define BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB 4
#define BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB  5
#define SOL_TCP 6
#define TCP_USER_TIMEOUT 18

SEC("sockops")
int bpf_barkan_l7(struct bpf_sock_ops *skops) {
    int op = skops->op;
    
    // Uruchomienie reguły w momencie 100% zestawienia połączenia
    if (op == BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB || op == BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB) {
        
        // Zabezpieczenie przed timeoutowaniem SSH (22). Operujemy tylko na L7.
        __u32 port = skops->local_port;
        if (port == 80 || port == 443 || port == 9494) {
            
            // BEZWZGLĘDNA GILOTYNA CZASOWA: 5 SEKUND
            // Jądro wyśle sprzętowy pakiet RST i zniszczy gniazdo w epoll
            // jeśli bufor klienta uśnie na ponad 5 sekund. Zero asymetrii CPU.
            int timeout = 5000; 
            bpf_setsockopt(skops, SOL_TCP, TCP_USER_TIMEOUT, &timeout, sizeof(timeout));
        }
    }
    return 0;
}
char _license[] SEC("license") = "GPL";
