# BARKAN V3: Stealth Auth & Distributed Ingress Shield

## CORE PRINCIPLE: DISTRIBUTED NOISE REDUCTION
Barkan V3 is not a monolithic L3/L4 filter. It is a distributed defense matrix (XDP + Sysctl + SockOps) operating across three separate layers of the network stack. It enforces strict deterministic limits and cryptographic stealth authentication before traffic reaches the application layer.

## DEFENSE VECTORS & REPOSITORY STRUCTURE
* **[L0] STEALTH UDP AUTH (src/xdp_barkan_v3.c):** An invisible port-knocking mechanism. Intercepts UDP packets on port 9494, verifies a 64-bit node_id payload, updates a pinned BPF map (authorized_nodes), and executes an immediate XDP_DROP. Remains completely invisible to external scanners. Operates at ~20ns per packet in native XDP mode (requires driver support). Falls back to generic XDP on NICs without native support — functional, higher latency. All non-UDP-9494 traffic safely passes to the OS kernel.
* **[L4] TCP KERNEL SHIELD (sysctl/99-barkan-tuning.conf):** Volumetric capacity protection. Relies on aggressive Linux kernel hardening (strict SYN cookies, expanded backlog limits) to absorb and neutralize TCP/SYN floods, effectively offloading stateful inspection from the XDP layer.
* **[L7] SOCKOPS GUILLOTINE (src/sockops_barkan.c):** Connection state injection via cgroupv2. Upon full TCP connection establishment on ports 80, 443, or 9494, BPF injects an absolute 5000ms TCP_USER_TIMEOUT. Idle or dormant sockets are destroyed via TCP RST by the kernel, requiring zero CPU cycles from the user-space application.

## BUILD
    make

## DEPLOYMENT ARCHITECTURE
Dependencies: clang, llvm, bpftool, iproute2.
Deployment operates strictly via direct eBPF object injection into the Kernel Virtual File System (/sys/fs/bpf) and CgroupV2 hierarchy. The loading logic is executed by src/load_shield.sh linked to a persistent systemd oneshot service.

## LICENSE
GNU General Public License v2.0 — Kamil Robert Niedzielski, 2026
