# `TUN/TAP` introduction

## Intro

* <https://docs.kernel.org/networking/tuntap.html>
* <https://vtun.sourceforge.net/tun/faq.html>
* <https://en.wikipedia.org/wiki/TUN/TAP>

TUN/TAP 给用户态程序提供报文接收和转发的功能, 两者对比如下：

| 项目 | TUN | TAP |
|------|-----|-----|
| 层级 | L3（网络层 / IP 层） | L2（数据链路层 / MAC 层） |
| 处理的数据 | IP 数据包 | 以太网帧（Ethernet Frame） |
| 使用场景 | 点对点通信（如 VPN） | 模拟以太网设备，如桥接虚拟机网络 |
| 接口名称常见格式 | `tun0` , `tun1` , ... | `tap0` , `tap1` , ... |
| 数据内容 | 不含以太网头部，直接是 IP 包 | 含有以太网头部 |
| 对应协议 | IP | Ethernet |

In order to use the driver a program has to open `/dev/net/tun` and issue a
corresponding `ioctl()` to register a network device with the kernel.

## TUN

The TUN is Virtual Point-to-Point network device.

> 1.1 What is the TUN ?
> The TUN is Virtual Point-to-Point network device.
> TUN driver was designed as low level kernel support for
> IP tunneling. It provides to userland application
> two interfaces:
>
> - /dev/tunX - character device; 
> - tunX - virtual Point-to-Point interface.
>
> Userland application can write IP frame to /dev/tunX
> and kernel will receive this frame from tunX interface.
> In the same time every frame that kernel writes to tunX
> interface can be read by userland application from /dev/tunX
> device.

## TAP

The TAP is a Virtual Ethernet network device.

<https://en.wikipedia.org/wiki/Test_Anything_Protocol>

> The **Test Anything Protocol** (TAP) is a protocol for
> communicating between test logic, called a TAP producer.
>
> TAP driver was designed as low level kernel support for
> Ethernet tunneling. It provides to userland application
> two interfaces:
>
> - /dev/tapX - character device; 
> - tapX - virtual Ethernet interface.
>
> Userland application can write Ethernet frame to /dev/tapX
> and kernel will receive this frame from tapX interface.
> In the same time every frame that kernel writes to tapX
> interface can be read by userland application from /dev/tapX
> device.

## 常见使用场景

1. **VPN (Virtual Private Network)**
   * TUN devices are commonly used in VPN software (e.g., OpenVPN) to tunnel IP packets between networks.

2. **Virtual Machine Networking**
   * TAP devices are used to bridge virtual machines to the host network, allowing them to communicate as if they were on the same Ethernet network.

3. **Network Simulation**
   * TUN/TAP devices are used in network simulation tools to test and debug network protocols.

4. **Custom Network Applications**
   * Developers can use TUN/TAP to create custom network applications, such as packet analyzers or traffic generators.

## 调试TUN/TAP

常见的方法有：

1. **Using `tcpdump`**
   You can use `tcpdump` to capture packets on TUN/TAP devices:

   ```bash
   sudo tcpdump -i tun0
   sudo tcpdump -i tap0
   ```

2. **Checking Device Status**
   Use `ip link show` to check the status of TUN/TAP devices:

   ```bash
   ip link show tun0
   ip link show tap0
   ```

3. **Verifying Packet Flow**
   Write a simple program to read/write packets from the TUN/TAP device and log the data for debugging.

4. **Kernel Logs**
   Check kernel logs for errors related to TUN/TAP:

   ``bash
   mesg | grep tun
   ``
