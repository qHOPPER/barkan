#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>
struct node_auth { __u64 base_seed; __u64 last_resonance; };
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 496);
    __type(key, __u64);
    __type(value, struct node_auth);
    __uint(pinning, LIBBPF_PIN_BY_NAME);
} authorized_nodes SEC(".maps");
SEC("xdp")
int bunker_shield(struct xdp_md *ctx) {
    void *data = (void *)(long)ctx->data;
    void *data_end = (void *)(long)ctx->data_end;
    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end) return XDP_PASS;
    if (eth->h_proto != __constant_htons(ETH_P_IP)) return XDP_PASS;
    struct iphdr *ip = data + sizeof(*eth);
    if ((void *)(ip + 1) > data_end) return XDP_PASS;
    if (ip->protocol == 17) {
        struct udphdr *udp = (void *)ip + sizeof(*ip);
        if ((void *)(udp + 1) > data_end) return XDP_DROP;
        if (udp->dest == __constant_htons(9494)) {
            if ((void *)udp + sizeof(struct udphdr) + sizeof(__u64) > data_end) return XDP_PASS;
            __u64 node_id = *(__u64 *)(udp + 1);
            struct node_auth *auth = bpf_map_lookup_elem(&authorized_nodes, &node_id);
            if (auth) {
                auth->last_resonance = bpf_ktime_get_ns();
                return XDP_DROP; 
            }
        }
    }
    return XDP_PASS;
}
char _license[] SEC("license") = "GPL";
