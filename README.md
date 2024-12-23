# Ensea-SH-TP2

Cédric JOVENET, Marin KERBORIOU
2G1 TP3

## Objectives : Develop a TFTP client using RFC specifications and Wireshark captures

In this lab, we will create a program that allows sending and receiving files with a server using a simple protocol called TFTP (Trivial File Transfer Protocol). This protocol is often used for tasks such as installing operating systems on computers over a network or updating equipment like routers. We need to implement two commands: one for downloading a file from the server (gettftp) and another for uploading a file to the server (puttftp). We will learn how to send the correct requests to the server, receive responses, and test everything using Wireshark to observe what happens on the network.


## How to use

### commands

We open 3 terminals:
- 1st terminal:
  - you go to the folder where there is the server. For us, we do: `cd Ensea-SH-TP2` then `cd server`.
  - We open wireshark `wireshark`. If you don't have the permission, do `sudo wireshark`.
  - It will open Wireshark, then you go to capture -> interfaces -> you select 'any' -> then click on 'start'
  - On the capture mode, go to the filter bar and write 'udp.port==1069'
- 2nd terminal:
  - you go to the folder where there is the server. For us, we do: `cd Ensea-SH-TP2` then `cd server`.
  - you start the server: `./go.sh`
  - If you don't have the permission, do: `chmod u+x tftpd` and `chmod u+x go.sh`, then  `./go.sh`
- 3rd terminal:
  - you go to the folder where there is the code: `cd Ensea-SH-TP2`
  - To launch the code, choose a file: `gcc -o q4 q4.c` then `./q4.c`

Now, on Wireshark, you should see a new line


