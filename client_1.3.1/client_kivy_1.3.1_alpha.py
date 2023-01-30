import socket
import threading
import os

from kivy.app import App
from kivy.clock import Clock,mainthread
from kivy.uix.screenmanager import ScreenManager, Screen, SlideTransition
from kivy.uix.filechooser import FileChooserIconView
import time

host_ip = "10.22.24.238"
ftp_port = 5050
cli_port = 5000

class client():
    def __init__(self):
        self.page1 = ''
        self.page2 = ''
        try:
            self.s = socket.socket()
        except socket.error() as msg:
            print("Socket error ::",msg)
    def bindto(self,host,port):
        self.host = host_ip
        self.port = cli_port
        try:
            self.s.connect((self.host,self.port))
        except socket.error() as msg:
            print("Bind error ::",msg)
    def input(self,inputs):
        self.s.send(str.encode(inputs))
    def output(self):
        while 1:
            data = self.s.recv(2048)
            # print(data.decode("utf-8"))
            self.page2.create_msg(data.decode("utf-8"))
    def start(self):
        outputi = threading.Thread(target=self.output)
        outputi.daemon = True
        outputi.start()
    def close(self):
        self.s.close()

class ftp():
    def __init__(self,label):
        self.label = label
        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error() as msg:
            print("FTP Socket error ::",msg)
    def bindto(self,host,port):
        self.host = host_ip
        self.port = ftp_port
        try:
            self.s.connect((self.host,self.port))
        except socket.error() as msg:
            print("FTP Bind error ::",msg)
    def send(self,path):
        try:
            file = open(path,'rb')
        except:
            self.label.text = "Invalid Path..."
            return 0
 
        size = os.path.getsize(path)
        name = path.split("/")[-1].split("\\")[-1]

        self.s.send(str.encode("/upload"))
        time.sleep(0.1)
        # self.s.send(str.encode(name))
        request = ""+name+" "+str(size)
        # print(request)
        # self.s.send(str.encode(str(size)))
        stat1 = self.s.send(str.encode(request))
        time.sleep(0.1)

        self.label.text = "Sending..."
        stat2 = self.s.send(file.read())
        # if(int(self.s.recv(1).decode('utf-8'))==1):
        self.label.text = "Successfully uploaded..."
    def receive(self,name):
        self.s.send(str.encode("/download"))
        time.sleep(0.1)
        self.s.send(str.encode(name))
        time.sleep(0.1)
        if(int(self.s.recv(1).decode('utf-8'))==1):
            size = int(self.s.recv(10).decode('utf-8'))
            time.sleep(2)
            file = open("./database/"+name,'wb+')
            content = self.s.recv(size)
            file.write(content)
            file.close()
        else:
            print("Unsuccessfull")
    def start(self):
        outputi = threading.Thread(target=self.receive)
        outputi.daemon = True
        outputi.start()
    def close(self):
        self.s.close()

class chat(Screen):
    def __init__(self, client, **kwargs):
        super(Screen, self).__init__(**kwargs)
        self.file = ''
        self.client = client
    def on_press(self):
        if(len(self.text.text)!=0):
            self.create_msg("YOU:"+self.text.text)
            self.client.input(self.text.text)
            self.text.text = ""
    def file_send(self):
        self.manager.current = 'fileselector'
    def check(self,value):
        self.download(value.split(" ")[0])
        print(value)
    def download(self,name):
        s = ftp(self.file)
        s.bindto(host_ip,ftp_port)
        s.receive(name)
        # self.cla.create_msg("YOU have shared the document [ref="+path.split("/")[-1].split("\\")[-1]+"]"+path.split("/")[-1].split("\\")[-1]+"[/ref]")
        # self.client.input("shared the document [ref="+path.split("/")[-1].split("\\")[-1]+"]"+path.split("/")[-1].split("\\")[-1]+"[/ref]")
        s.close()
        del s

    @mainthread
    def create_msg(self,message):
        new_msg = message.split(":")
        if(len(new_msg)==2):
            self.chat.text += '\n'
            if(new_msg[0]=="YOU"):
                self.chat.text += f'[color=FBB600]{new_msg[0]}[/color]'
            else:
                self.chat.text += f'[color=95FB00]{new_msg[0]}[/color]'
            new_msg[1] = new_msg[1].rstrip('\n')
            self.chat.text += ' --> '+new_msg[1]
            return
            
        self.chat.text += '\n'+message.rstrip('\n')

class welcome(Screen):
    def __init__(self, client, **kwargs):
        super(Screen, self).__init__(**kwargs)
        self.client = client
    def next(self):
        if(len(self.nami.text)>10):
            self.lab.text = "Text size must not exceed 10 characters"
            self.nami.text = ""
            return
        if(len(self.nami.text)==0):
            self.lab.text = "Username cannot be NULL"
            self.nami.text = ""
            return
        if(" " in self.nami.text):
            self.lab.text = "Spaces cannot be used"
            self.nami.text = ""
            return
        self.client.input(self.nami.text)    
        if(int(self.client.s.recv(1).decode('utf-8'))==0):
            self.lab.text = "Username already exist!!"
            self.nami.text = ""
            return
        self.manager.current = 'chat'
        self.client.start()

class errorpage(Screen):
    def __init__(self, message, **kwargs):
        super(Screen, self).__init__(**kwargs)
        self.lab.text = message

class fileselector(Screen):
    def __init__(self,client,obj,**kwargs):
        super(Screen, self).__init__(**kwargs)
        self.client = client
        self.cla = obj
    def backchat(self):
        self.manager.current = 'chat'
    def send(self,path):
        s = ftp(self.path)
        s.bindto(host_ip,ftp_port)
        s.send(path)
        self.cla.create_msg("YOU have shared the document [ref="+path.split("/")[-1].split("\\")[-1]+"]"+path.split("/")[-1].split("\\")[-1]+"[/ref]")
        self.client.input("shared the document [ref="+path.split("/")[-1].split("\\")[-1]+"]"+path.split("/")[-1].split("\\")[-1]+"[/ref]")
        s.close()
        del s

class messenger(App):
    def build(self):
        try:
            conn = client()
            conn.bindto(host_ip,cli_port)
        except:
            return errorpage(message="Error in connecting\n\nTry again later")

        self.window = ScreenManager()

        page1 = welcome(name="welcome",client=conn)
        page2 = chat(name="chat",client=conn)
        page3 = fileselector(name="fileselector",client=conn,obj=page2)

        conn.page1 = page1
        conn.page2 = page2
        conn.page3 = page3

        page2.file = page3

        self.window.add_widget(page1)
        self.window.add_widget(page2)
        self.window.add_widget(page3)

        return self.window

messenger().run()