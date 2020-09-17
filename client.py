
import socket
import time
import struct


def getErrorInfo(error):
    argsLen = len(error.args)
    if argsLen == 1:
        return str(error.args[0])
    elif argsLen > 1:
        return '[' + str(error.args[0]) + '] ' + error.args[1]
    else:
        return 'unknown error.'


def test(host, port):
    socket.setdefaulttimeout(2)
    errorMsg = ""
    result = -1
    s = None
    while 1:
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        except socket.error as msg:
            errorMsg = getErrorInfo(msg)
            s = None
            break
        try:
            s.connect((host, port))
        except socket.error as msg:
            s.close()
            s = None
            errorMsg = getErrorInfo(msg)
            break
        try:
            while 1:
                print("请输入要发送的内容：")
                msg = input()
                s.send(msg.encode('utf-8'))
                buf = s.recv(1024)
                del msg
                print('recv msg:' + buf.decode('utf-8'))
        except socket.error as msg:
            s.close()
            s = None
            errorMsg = getErrorInfo(msg)
            break
        break
    if s is None:
        return errorMsg
    else:
        s.close()
        return result


if __name__ == '__main__':
    print(test('192.168.16.101', 9041))