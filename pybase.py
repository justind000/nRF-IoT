from nrf24 import NRF24
import struct
import time
import serial, sys, string
import httplib

domain = "node.ntdll.net"
emoncmspath = "emoncms"
apikey = "46f3d279c7d06141e90d88db607448e5"
nodeid = 10
conn = httplib.HTTPConnection(domain)

cnt = [0,0,0,0,0,0,0,0,0,0]
cntID = 0

def DupID(id):
	#this function keeps the last 10 header.IDs, then searches through them all
	#when cntID reaches 10, it rolls over to 0 again
	#if a match is found, it returns true, false otherwise
    global cntID
    found = False
    i = 0
    while i < 10:
        if cnt[i] == id:
            found = True
            break
        i += 1
    if cntID < 10:
        cnt[cntID] = id
    else:
        cntID = 0
        cnt[cntID] = id
    cntID += 1
    return found

#pipes = [[0xaa, 0x00, 0x00, 0x00, 0x02]]
pipes = [[0xe7, 0xe7, 0xe7, 0xe7, 0xe7], [0xc2, 0xc2, 0xc2, 0xc2, 0xc2]]

radio = NRF24()
radio.begin(1, 0, "P8_15", "P8_16") #Set CE and IRQ pins
radio.setRetries(15,15)
radio.setPayloadSize(28)
radio.setChannel(0x4c)
radio.setDataRate(NRF24.BR_1MBPS)
radio.setPALevel(NRF24.PA_MAX)

radio.openWritingPipe(pipes[1])
radio.openReadingPipe(1, pipes[0])

radio.startListening()
radio.stopListening()
radio.printDetails()

while 1:
	#Wait for data
	pipe =[0]
	while not radio.available(pipe):
		time.sleep(10000/1000000.0)

	#Receive Data
	recv_buffer = []
	radio.read(recv_buffer)
	try:
		conn = httplib.HTTPConnection(domain)		#new httplib object everytime for stability. if the request fails, it will continue to fail without restarting.
		header = struct.unpack("llllfff", ''.join(chr(c) for c in recv_buffer))
		csv = str(header[4]) +','+str(header[5])+','+ str(header[6])
		conn.request("GET", "/"+emoncmspath+"/input/post.json?apikey="+apikey+"&node="+str(nodeid)+"&csv="+csv)
		response = conn.getresponse() 
		print 'got ' +csv +' from ' + hex(header[3])
		print response.read() 
	except: # catch *all* exceptions
		print sys.exc_info()[0]
	results = DupID(header[3]) 
