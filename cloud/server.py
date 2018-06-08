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
t = 0
value_flag = 0
value = []
bufsize = 0
while True:
    #bufsize = clientsock.recv(4)
    #print(bufsize)
    rcvmsg = clientsock.recv(924)
    if not rcvmsg:
        break
    else:
        value = []
        print("変態")
        BUFF = rcvmsg.split("\n")
        print(BUFF)
        for i in BUFF:
            i = i.split(",")
            if(len(i)>1):
                i = i[1].split("value=")
                value.append(i[1])
                #print(i)
        #print(value)
        #print("#########"+str(len(value)))
        #value_flag = BUFF.find('value=')
        #if(value_flag != -1):
        #    print(str(value_flag)+rcvmsg)
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
