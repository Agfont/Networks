# Networks

A comprehensive collection of laboratory exercises and theoretical materials for computer networks study.

## Laboratory Overview

| Exercise | Folder | Description |
|----------|--------|-------------|
| [EP0](#ep0---network-analysis) | `Laboratory/EP0/` | Network packet analysis and PCAP file examination |
| [EP1](#ep1---mqtt-server) | `Laboratory/EP1/` | MQTT server implementation and performance testing |
| [EP2](#ep2---secure-game-server) | `Laboratory/EP2/` | Secure client-server game with encryption |
| [EP3](#ep3---firewall-and-switch) | `Laboratory/EP3/` | Firewall implementation and network switching |
| [EP4](#ep4---advanced-networking) | `Laboratory/EP4/` | Advanced networking concepts |

---

### EP0 - Network Analysis

**Location:** `Laboratory/EP0/`

This exercise focuses on network packet analysis using PCAP files. It includes:
- `pergunta.pcapng` - Primary packet capture file for analysis
- `ep0-arthur_font_gouveia/` - Solutions and detailed responses
  - `pergunta2.pcapng` - Additional packet capture file
  - `respostas.txt` - Answers and analysis results

**Topics Covered:**
- Packet capture and analysis
- Network traffic examination
- Protocol identification

---

### EP1 - MQTT Server

**Location:** `Laboratory/EP1/`

Implementation of an MQTT server with performance analysis. Includes:
- `bash.sh` - Utility bash scripts
- `ep1-arthur_font_gouveia/` - Solution implementation
  - `ep1-mqtt-server.c` - MQTT server implementation
  - `desempenho.c` - Performance testing module
  - `Makefile` - Build configuration
  - `README.md` - Detailed exercise documentation
- `Prints/` - Test output and results

**Topics Covered:**
- MQTT protocol implementation
- Server socket programming
- Performance benchmarking
- C networking fundamentals

---

### EP2 - Secure Game Server

**Location:** `Laboratory/EP2/`

A complete client-server game implementation with encryption and security. Includes:
- `ep2-arthur-lucas` - Solution directory with `README.md`
    - `main.py` - Main entry point
    - `client/` - Client-side code
        - `client.py` - Client implementation
        - `game.py` - Game logic
    - `server/` - Server-side code
        - `server.py` - Server implementation
        - `connection.py` - Connection management
        - `user.py` - User management
        - `reboot.py` - Server restart utilities
        - `data/` - Data storage
    - `perm/` - Cryptographic keys
        - `sk.pem` - Private key
        - `pk.pem` - Public key
- `scripts/` - Testing and automation
  - `match.sh` - Game match runner
  - `player1`, `player2` - Player scripts

**Topics Covered:**
- Client-server architecture
- Encryption and SSL/TLS
- Public key cryptography
- Python socket programming
- Game protocol design

---

### EP3 - Firewall and Switch

**Location:** `Laboratory/EP3/`

Network firewall and switch implementation with configuration rules. Includes:
- `firewall.py` - Firewall implementation
- `switch.py` - Network switch implementation
- `rules.json` - Firewall rules configuration
- `mac0352-relatorio-ep3.tex` - LaTeX report document
- `ep3-arthur_font_gouveia/` - Solution directory with `README.md`

**Topics Covered:**
- Firewall rule processing
- Network packet filtering
- Layer 2 switching
- Network security policies
- Rule configuration and management

---

### EP4 - Network Vulnerability Exploitation and Analysis

**Location:** `Laboratory/EP4/`

This exercise focuses on exploring and understanding real-world network security vulnerabilities. The objective is to identify a programming flaw that leads to a vulnerability, demonstrate and explain its exploitation and patch.

**Vulnerability Studied:** [CVE-2020-8794](https://nvd.nist.gov/vuln/detail/CVE-2020-8794)

- **Type**: Out-of-bounds buffer read in DNS protocol parsing
- **Affected Component**: DNS response message handling
- **Root Cause**: Improper boundary checking when parsing three-digit DNS response codes without optional space and text
- **Attack Vector**: Malicious DNS server (client-side) or malicious DNS client (server-side)
- **Impact**: Buffer overflow leading to information disclosure or potential code execution

**Files:**
- `Arthur_Lucas_EP4.pdf` - Complete study and analysis report
- `EP4.txt` - Exercise description and requirements
- `Roteiro.pdf` - Presentation overview
- Supporting images:
  - `codigo.png` - Vulnerable code snippet
  - `patch.png` - Patched/fixed code
  - `envelope.png` - DNS message envelope structure
  - `env malicioso.png` - Malicious envelope construction

**Topics Covered:**
- Network security vulnerabilities (CVE analysis)
- DNS protocol exploitation techniques
- Buffer overflow vulnerabilities
- Out-of-bounds memory access
- Vulnerability remediation and secure patching
- Root cause analysis of programming flaws

---

## Related Repositories

- EP2 Repository: https://github.com/Agfont/Networks-EP2