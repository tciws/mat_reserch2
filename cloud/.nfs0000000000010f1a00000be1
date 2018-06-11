# -*- coding:utf-8 -*-
import socket

host = "cs-d10" #お使いのサーバーのホスト名を入れます
port = 55555 #クライアントと同じPORTをしてあげます

serversock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
serversock.bind((host,port)) #IPとPORTを指定してバインドします
serversock.listen(10) #接続の待ち受けをします（キューの最大数を指定）

print 'Waiting for connections...'
clientsock, client_address = serversock.accept() #接続されればデータを格納
rcvmsg = "NULL"
while(1):
    rcvmsg = clientsock.recv(1024)
    if rcvmsg == "END":
        break
    else:
        print(rcvmsg)
#    print 'Received -> %s' % (rcvmsg)
#    if rcvmsg == '':
#      break
#    print 'Type message...'
#    s_msg = raw_input()
#    if s_msg == '':
#      break
#    print 'Wait...'
#
#    clientsock.sendall(s_msg) #メッセージを返します
clientsock.close()
