# python Tornado server for Websockets

import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import socket

'''
This is a simple Websocket Echo server that uses the Tornado websocket handler.
Please run `pip install tornado` with python of version 2.7.9 or greater to install tornado.
This program will echo back the reverse of whatever it recieves.
Messages are output to the terminal for debuggin purposes. 
''' 

class WSHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print 'new connection'

    def on_message(self, message):
        print 'message received: %s' % message
        # Reverse Message and send it back
        print 'sending back message: %r' % (message)
        
        if message == 'wykres-wymiennik': # tutaj chcemy odebrac caly wykres
            #chartFile = open('wykres-wymiennik.txt', 'r')
            chartFile = open('data.csv', 'r')
            chart = chartFile.read()
            chartFile.close()
            print 'dane do wykresu: %r' % chart
            self.write_message(str(chart))
    
        else: # nastawy
            params = message.split(';')
            
            if params[0] == 'nastawy-budynek':
                target = open('nastawy_bud.txt', 'w')
                target.write(str(params[1]));
                target.close();
                self.write_message(message)

            elif params[0] == 'nastawy-wymiennik':
                # wyslij dane do serwera c przez plik
                target = open('nastawy_wym.txt', 'w');
                target.write(str(params[1]));
                target.close();
                self.write_message(message)
            else:
                print 'nieznane zapytanie od przegladarki www!'

    def on_close(self):
        print 'connection close'

    def check_origin(self, origin):
        return True

application = tornado.web.Application([
    (r'/ws', WSHandler),
])


if __name__ == "__main__":
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(8888)
    myIP = socket.gethostbyname(socket.gethostname())
    print '*** Websocket Server Started at %s ***' % myIP
    tornado.ioloop.IOLoop.instance().start()

