#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <termios.h>
#include <signal.h>

struct Frame{
        unsigned char PLATFORM_ID;       
        unsigned char DATA_LENGTH;
        unsigned char COMMAND;
        unsigned char DATA_FRAME;
        unsigned char CRC;
        unsigned char ERR_STATUS;
};

/*void genFrame (struct msgFrame * ){
        
	Frame.PLATFORM_ID = 0X00;
	Frame.DATA_LENGTH =0X01;
	Frame.COMMAND = 0X01;
	Frame.CACK;
	Frame.DATA_FRAME = 0X55;
        Frame.CRC = 0X00;
	Frame.DACK;
        Frame.ERR_STATUS;
}*/

void signalHandler(int signum){

        int fd,wr;
        const char STOP_FRAME  =0x50;

        printf("hello/n");

	/*--Open /dev/ttyS0--*/
   	fd=open("/dev/ttyS0",O_RDWR|O_SYNC|O_NOCTTY|O_NDELAY,0666);
	if(fd==-1)
        {
		perror("open:");
	}
	else
	{
       		fcntl(fd,F_SETFL,0);
   	} 

	/*--Write stop byte--*/ 
	wr = write(fd,&STOP_FRAME,1);
      
	/*--close /dev/ttySO--*/
	close(fd);

        exit(1);
}

void errorStatus(unsigned char CACK){

        switch(CACK)
        {
           case 0x00:printf("success\n");
                     break;

           case 0x01:printf("failed1\n");
                     break;

           case 0x02:printf("failed2\n");
                     break;

           case 0x03:printf("failed3\n");
                     break;

           case 0x04:printf("failed4\n");
                     break;

           case 0x05:printf("failed5\n");
                     break;

           case 0x06:printf("failed6\n");
                     break;

           default:printf("failed7\n");
                     break;
         }
       return;
}

int main()
{ 
	struct Frame msgFrame;

	int fd,wr,rd,read_op;
       
	unsigned char CACK,DACK;
 
        const char START_FRAME =0x57;
	msgFrame.PLATFORM_ID = 0X00;
	msgFrame.DATA_LENGTH =0X01;
	msgFrame.COMMAND = 0X00;
	msgFrame.DATA_FRAME = 0X55;
        msgFrame.CRC = 0X00;
        msgFrame.ERR_STATUS;
        const char STOP_FRAME  =0x50;

	/*--Open /dev/ttyS0--*/
   	fd=open("/dev/ttyS0",O_RDWR|O_SYNC|O_NOCTTY|O_NDELAY,0666);
	if(fd==-1)
        {
		perror("open:");
	}
	else
	{
       		fcntl(fd,F_SETFL,0);
   	} 
  
        if(signal(SIGALRM,signalHandler)==SIG_ERR)
        {
            perror("signal");
        }
        					 
	if(msgFrame.COMMAND & 0X10 == 0X10)
	{
		read_op = 1;
		msgFrame.DATA_FRAME = malloc((msgFrame.DATA_LENGTH * sizeof(char)));
		if(!msgFrame.DATA_FRAME)
                {
			perror("Malloc");
                        exit(1);
                }
	}


	/*--Write start byte--*/
	wr = write(fd,&START_FRAME,1);

        /*--Write Platform ID--*/
        wr = write(fd,&msgFrame.PLATFORM_ID,1);

        /*--Write Data Length--*/
        wr = write(fd,&msgFrame.DATA_LENGTH,1);

        /*--Write Command--*/
        wr = write(fd,&msgFrame.COMMAND,1);

        sync();

	/*--Config ALARM--*/
	alarm(10);        

//        printf("hello/n");

        /*--Read Control ACK--*/
        rd = read(fd,&CACK,1); 
        if(rd==-1)
        {
          perror("Read") ;
        }               

	/*--Disable ALARM--*/        
	alarm(0);
        
        /*--Call errorStatus fn for CACK Status--*/
        errorStatus(CACK);

        if(CACK & 0XFF == 0X00)
        {
              	/*--Check CACK=0X00?--*/
		if(read_op != 1)
		{	
		
			/*--Write Data Frame--*/
			wr = write(fd,&msgFrame.DATA_FRAME,sizeof(msgFrame.DATA_FRAME));
		
        	        /*--Write CRC--*/
			wr = write(fd,&msgFrame.CRC,1);

        	        /*--Configure ALERM for 10 sec--*/
			alarm(10);

     			sync();

        		/*--Read Data ACK--*/
        		rd = read(fd,&DACK,1);

        	        /*--Disable ALARM--*/
			alarm(0);
                 
        	        if(DACK==0X00)
        	        {
        	              printf("Data sends Successfully\n");
        	        }
            
        	        else
        	        {
        	              printf("Data sends Failed\n");
        	        }

                        /*  kill(getpid(),14);  */

	                /*--Write stop byte--*/ 
	                wr = write(fd,&STOP_FRAME,1);
      
	                /*--close /dev/ttySO--*/
	                close(fd);
		}

		else
		{
        	        /*--Configure ALARM for 10 sec--*/
			alarm(10);

       			sync();

			/*--Read Data Frame--*/
			rd = read(fd,&msgFrame.DATA_FRAME,sizeof(msgFrame.DATA_FRAME));

        	        /*--Disable ALARM--*/
			alarm(0);
	
        	        /*--Configure ALARM for 10 sec--*/
			alarm(10);

                        sync();
		
 			/*--Read CRC Byte--*/
			rd = read(fd,&msgFrame.CRC,1);
	
        	        /*--Disable ALARM--*/
			alarm(0);

        	 unsigned char DACK=0X00;
		
			/*--Write DACK--*/
			wr = write(fd,&DACK,1);

      		       /*  kill(getpid(),14);  */

	               /*--Write stop byte--*/ 
	               wr = write(fd,&STOP_FRAME,1);
      
	               /*--close /dev/ttySO--*/
	               close(fd);
        	}


	}



}   
