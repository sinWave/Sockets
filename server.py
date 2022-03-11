import socket
from sys import getsizeof

# ----------------------- MAIN -----------------------

def main():
    HOST = 'localhost'
    PORT = 8080

    whitelist = {'oingo' : '1', 'boingo' : '2', 'forreal' : '123'}

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))

        s.listen()

        conn, addr = s.accept()

        with conn:
            communicate(conn, addr, whitelist)

# ----------------------- COMM -----------------------

def communicate(conn, addr, whitelist):
    print(f'\nConnected by: {addr}.')
    print("Waiting for client...")

    # Client requests message size
    while True:
        tBytes = conn.recv(1024)

        if not tBytes: break

    print("Client requests message size.")

    t, size = encoder(whitelist)

    print(f"Whitelist size: {size.decode('utf-8')}")

    sentBytes = 0
    totalBytes = len(size)
    tBytes = 0

    # Send
    while True:
        tBytes = conn.send(size[sentBytes:])

        if tBytes < 0: raise RuntimeError("Socket connection broken.")

        if tBytes == 0: break

        if sentBytes >= totalBytes: break
        
        sentBytes = sentBytes + getsizeof(tBytes)

    print("Whitelist size sent.")

    # Client requests actual message
    while True:
        tBytes = conn.recv(totalBytes)

        if not tBytes: break

    print("Client requests actual whitelist.")

    sentBytes = 0
    totalBytes = len(t)
    tBytes = 0

    # Send
    while True:
        tBytes = conn.send(t[sentBytes:])

        if tBytes < 0: raise RuntimeError("Socket connection broken.")

        if tBytes == 0: break

        if tBytes >= totalBytes: break
        
        sentBytes = sentBytes + getsizeof(tBytes)

    print("Whitelist sent.")

    return

# ----------------------- ENCO -----------------------

def encoder(whitelist):
    t = '' # Simplify dict to list object
    for url in whitelist:
        t += url
        t += ','
        t += whitelist[url]
        t += ','

    return (t[:-1]).encode('utf-8'), str(getsizeof(t)).encode('utf-8')

if __name__ == '__main__':
    main()
