# Ensea-SH-TP2

Cédric JOVENET, Marin KERBORIOU
2G1 TP3

## Objectives : Develop a TFTP client using RFC specifications and Wireshark captures

In this lab, we will create a program that allows sending and receiving files with a server using a simple protocol called TFTP (Trivial File Transfer Protocol). This protocol is often used for tasks such as installing operating systems on computers over a network or updating equipment like routers. We need to implement two commands: one for downloading a file from the server (gettftp) and another for uploading a file to the server (puttftp). We will learn how to send the correct requests to the server, receive responses, and test everything using Wireshark to observe what happens on the network.


## How to use

### Terminals
- 1st terminal:
  - you go to the folder where there is the server. For us, we do: `cd Ensea-SH-TP2` then `cd server`.
  - We open wireshark `wireshark`. If you don't have the permission, do `sudo wireshark`.
  - It will open Wireshark, then you go to capture -> interfaces -> you select 'any' -> then click on 'start'
  - On the capture mode, go to the filter bar and write 'udp'
- 2nd terminal:
  - you go to the folder where there is the server. For us, we do: `cd Ensea-SH-TP2` then `cd server`.
  - you start the server: `./go.sh`
  - If you don't have the permission, do: `chmod u+x tftpd` and `chmod u+x go.sh`, then  `./go.sh`
- 3rd terminal:
  - you go to the folder where there is the code: `cd Ensea-SH-TP2`
  - To launch the code, choose a file: `gcc -o q4 q4.c` then for example `./q4 localhost zeros256`

### Wireshark
- Now, on Wireshark, you should see 3 new lines : the first for the ReadRequest, the second for the data packet, and the third for the acknowledgement.
The protocole is UDP and you can see that the first line is sent to the port 1069, because the server doesn't decode directly on this port.
- But you don't see the lines with those names at the start, so you need to right click on the lines, do 'decode as', then search 'TFTP' for it to show the lines properly.
The protocole is now TFTP.


