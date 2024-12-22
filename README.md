# Ensea-SH-TP2

Cédric JOVENET, Marin KERBORIOU
2G1 TP3

## Objectives : Develop a TFTP client using RFC specifications and Wireshark captures

In this lab, we will create a program that allows sending and receiving files with a server using a simple protocol called TFTP (Trivial File Transfer Protocol). This protocol is often used for tasks such as installing operating systems on computers over a network or updating equipment like routers. We need to implement two commands: one for downloading a file from the server (gettftp) and another for uploading a file to the server (puttftp). We will learn how to send the correct requests to the server, receive responses, and test everything using Wireshark to observe what happens on the network.


## How to use

### commands

We open 2 terminals, the first to test the functions of our local program (gettftp, getaddr ...)
the second one to connect on the esrver with the server file : `cat go.sh`
then we can communicate with the server on the first terminal.


### commands for q3 
`chmod u+x tftpd`
`chmod u+x go.sh`
`./go.sh`
