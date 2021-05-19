using System.Net;
using System.Net.Sockets;
using System;
using System.Collections.Generic;
using System.Collections;
using System.IO;
using System.Text;
using System.Threading.Tasks;

class Server{

       public string Name { get; set; }


  public static void Main(){

        IPAddress ipAddress = IPAddress.Parse("127.0.0.1");
        IPEndPoint remoteEP = new IPEndPoint(ipAddress,1234);

        Socket server = new Socket(remoteEP.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

        server.Bind(remoteEP);
        server.Listen(5);

        while (true)
        {
            Socket newconnection = server.Accept();
            IPEndPoint newclient = (IPEndPoint)newconnection.RemoteEndPoint;
            Task conn = new Task(() => Hnd(newconnection, newclient));
            conn.Start();
        }





    }
    /*
    public static bool OnPost()
        {
            if(ModelState.IsValid == false)
            {
                return false;
            }
            try
            {
                using (FileStream fs = System.IO.File.Create(@"http\Correspondance\"+ correspondence.Name + ".cr"))
                {
                    byte[] title = new UTF8Encoding(true).GetBytes(correspondence.Name);
                    fs.Write(title, 0, title.Length);
                    return true;
                }
            }
            catch
            {
                return false;
            }
            return false;
        }
        */



    public static void Hnd(Socket socket,IPEndPoint ip){

        Console.WriteLine("Connected with {0} at port {1}", ip.Address, ip.Port);

        byte[] buff = RecvHttp(socket);

        string http = Encoding.ASCII.GetString(buff);

        string[] data = SplitLines(http);

        string[] path = data[0].ToString().Split(' ');

        foreach (var item in path)
        {
            Console.WriteLine($"NETWORK :{item}");

        }

    
        if (path[1] == ".." || path[1].Contains("//"))
        {
            throw new Exception("nope");
        }
        //
        System.Console.WriteLine(path[1]);
        if(path[1] == "POST"){
            
            //Post();

        }

      try
       {
        
            if (path[1] == "/")
            {
                path[1] = "en.html";
            }
            if (path[1].StartsWith("/"))
            {

                path[1] = path[1].Substring(1);
            }


            byte[] d = new byte[1024]; 
            d = File.ReadAllBytes(path[1]);
            string ext = Path.GetExtension(path[1]);

            Dictionary<string, string> mim = new Dictionary<string, string>();
            mim.Add(".html", "text/html;charset=utf-8");
            mim.Add(".jpg", "image/vnd.sealedmedia.softseal.jpg");
            mim.Add(".mp4", "video/mp4");
            mim.Add(".gif", "image/vnd.sealedmedia.softseal.gif");

            string mtype = "application/binary";

            if (mim.ContainsKey(ext))
            {
                mtype = mim[ext];
            }
            ReturnHttp(socket, d, mime: mtype);
            socket.Dispose();

        }
        catch (Exception ex)
        {
            ReturnHttp(socket, b("None"), status: 404, statustxt: "Forbidden",
            mime: "text/plain;charset=utf-8");
            socket.Dispose();
        }




    }
    public static void ReturnHttp(Socket socket, byte[] data,int status = 200,string statustxt = "OK",string mime = "application/binary")
    {

        socket.Send(b($"HTTP/1.1 {status} {statustxt}\r\n"));
        socket.Send(b($"Content-Type: %s\r\n{mime}"));
        socket.Send(b($"Content-Length: {data.Length} \r\n"));
        socket.Send(b("\r\n"));
        socket.Send(data);
        socket.Dispose();
    }
    public static void Post(Socket socket, byte[] data,int status = 200){
    //POST /test HTTP/1.1
    //Host: foo.example
    //Content-Type: application/x-www-form-urlencoded
    //Content-Length: 27
    //field1=value1&field2=value2

    }


    public static byte[] RecvHttp(Socket sock){

        byte[] buff    = new byte[1024];
        byte[] cutbuff = new byte[1024];
        sock.Receive(buff);



        for (int i = 0; i < buff.Length; i++)
        {
            if (buff[i] == (char)13 && buff[i + 1] == (char)10 && buff[i +2] == (char)13 && buff[i + 3] == (char)10)
            {

                Array.Copy(buff, 0, cutbuff,0,buff.Length - (buff.Length - i));
                return cutbuff;
            }
        }

        sock.Dispose();
        return cutbuff;

    }
    public static string[] SplitLines(string s)
    {
        return s.Split(new[] { "\r\n", "\r", "\n" }, StringSplitOptions.None);
    }

    public static byte[] b(string ascii)
    {
        return Encoding.ASCII.GetBytes(ascii);
    }






}
