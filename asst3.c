/*
Student name : Khushi Bhat
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include "asst3.h"
#define BACKLOG 5

/***
    main program
***/
int main(int argc, char **argv)
{
    // validate if argument is provided. we need both server port to listen and the joke file
    if (argc != 3) {
        // print the error to stderr
        fprintf(stderr,"Usage: %s [port] [jokefile]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // call function validate the input . Error out if an error found
    if (validateInput (argv) != 0) {
        fprintf(stderr,"Usage: %s [port] [jokefile]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Collect all the jokes from the jokes file (argv[2])
    // Error out if there is an error in the jokes file
    if (getJokes (argv[2]) != 0) {
        fprintf(stderr,"Error in Jokes\n");
        exit(EXIT_FAILURE);
    }

    // now that input has been validated and jokes files read
    // we can call the server function to begin listening
    (void) server(argv[1]);
    return EXIT_SUCCESS;
}


/***
    validateInput function
    arguments  are the command line arguments recived
    returns 0 if all good , otherwise returns a non-zero
***/

int validateInput(char **input) {

    int port; // port
    char *file; // filename
    port = atoi(input[1]); // first argument is supposed to be port
    file = input[2];  // second argument is supposed to be the jokes files

    // check  whether port is 0  . return error if true
    if (port == 0) {
        fprintf(stderr,"Invalid port. Valid port is a number > 5000 < 65536.\n") ;
        return 1;
    }
    // check  whether port is in right range (5000-65536 )   . return error if not
    if (port < 5000 || port > 65536)  {
        fprintf(stderr,"Invalid port. Valid port is > 5000 < 65536.\n") ;
        return 1;
    }

    // check if the jokes file is readable. return error if not
    if (access(file,R_OK) != 0) {
        fprintf(stderr,"file %s is not accessible \n", file);
        return 1;
    }
    // all good if we are here . so return 0
    return 0;
}

/***
    getJokes function
    arguments  are is the jokes file, previously already checked for read access
    returns 0 if all good , otherwise returns a non-zero
***/
int getJokes (char *file) {
    FILE *fp;  // file pointer to the stream
    char line[MAX_MESSAGE_LEN+1]; // array to hold a setup or punch line
    int index=-1;  // index to keep track of number of jokes

    // open file  and get the file handler
    if ((fp = fopen(file,"r")) == NULL) {
        fprintf(stderr, "Cant open file %s\n", file);
        return 1;
    }

    // loop
    // joke file is expected to be in this format
    // setupline
    // punchline
    // <emptyline?
    // repeat next joke
    while (index < 100)  {  // max 100 jokes
        // read a line from the file, this is the setup .. break out if no more lines
        if (fgets (line, MAX_MESSAGE_LEN, fp)!=NULL ) {
            index++;
            line[strlen(line)-1] = '\0';
            myjokes[index].setup = (char *) malloc (strlen(line));
            strcpy (myjokes[index].setup , line);
        }
        else
            break;

        // read a line from the file, this is the punch line .. return 1 (error condition) if no more line..half done joke
        if (fgets (line, MAX_MESSAGE_LEN, fp)!=NULL ) {
            line[strlen(line)-1] = '\0';
            myjokes[index].punch = (char *) malloc (strlen(line));
            strcpy (myjokes[index].punch , line);
        }
        else {
            fprintf(stderr,"Joke file not built correctly. Not funny \n");
            return 1;
        }

        // read next line.. we expect this be an empty line.. but ignore it even if not empty.. no more validation done
        if (fgets (line, MAX_MESSAGE_LEN, fp)==NULL )
            break;
    }

    // all done , so close the file
    fclose(fp);
    // set up global count of totJokes
    totJokes = index+1;
    if (index >= 0) 
        return 0;
    fprintf(stderr,"Joke file not built correctly. Not funny \n");
    return 1;
}

/***
    server function
    Sets up the server using sockets 
    argument is the port to be used
***/

int server(char *port)
{
    struct addrinfo hint, *address_list, *addr;
    struct connection *con;
    int error, sfd;
    pthread_t tid;


    // initialize hints
    memset(&hint, 0, sizeof(struct addrinfo)); 
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    // setting AI_PASSIVE means that we want to create a listening socket

    // get socket and address info for listening port
    // - for a listening socket, give NULL as the host name (because the socket is on
    //   the local host)
    error = getaddrinfo(NULL, port, &hint, &address_list);
    if (error != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    // attempt to create socket
    for (addr = address_list; addr != NULL; addr = addr->ai_next) {
        sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        // if we couldn't create the socket, try the next method
        if (sfd == -1) {
            continue;
        }

        // if we were able to create the socket, try to set it up for
        // incoming connections;
        //
        // note that this requires two steps:
        // - bind associates the socket with the specified port on the local host
        // - listen sets up a queue for incoming connections and allows us to use accept
        if ((bind(sfd, addr->ai_addr, addr->ai_addrlen) == 0) &&
            (listen(sfd, BACKLOG) == 0)) {
            break;
        }

        // unable to set it up, so try the next method
        close(sfd);
    }

    if (addr == NULL) {
        // we reached the end of result without successfuly binding a socket
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(address_list);

    // at this point sfd is bound and listening
    //printf("Waiting for connection\n");
    for (;;) {
        // create argument struct for child thread
        con = malloc(sizeof(struct connection));
        con->addr_len = sizeof(struct sockaddr_storage);
                // addr_len is a read/write parameter to accept
                // we set the initial value, saying how much space is available
                // after the call to accept, this field will contain the actual address length

        // wait for an incoming connection
        con->fd = accept(sfd, (struct sockaddr *) &con->addr, &con->addr_len);
                // we provide
                // sfd - the listening socket
                // &con->addr - a location to write the address of the remote host
                // &con->addr_len - a location to write the length of the address
                //
                // accept will block until a remote host tries to connect
                // it returns a new socket that can be used to communicate with the remote
                // host, and writes the address of the remote host into the provided location

        // if we got back -1, it means something went wrong
        if (con->fd == -1) {
            perror("accept");
            continue;
        }

        // spin off a worker thread to handle the remote connection
        error = pthread_create(&tid, NULL, joker, con);

        // if we couldn't spin off the thread, clean up and wait for another connection
        if (error != 0) {
            fprintf(stderr, "Unable to create thread: %d\n", error);
            close(con->fd);
            free(con);
            continue;
        }

        // otherwise, detach the thread and wait for the next connection request
        pthread_detach(tid);
    }

    // never reach here
    return 0;
}

/***
    joker function
    handles interaction with client in its own thread
    argument is the connection structure
***/
void *joker(void *arg)
{
    char host[100], port[10]; // variables to store the client host and port
    char buf[MAX_MESSAGE_LEN+1], mybuf[MAX_MESSAGE_LEN+1];  // variable for the message recv
    struct connection *c = (struct connection *) arg; // the connection structure which came in as an arg ,we cast it
    int error, nread;
    int jokeIndex; //index into the joke array for the joke used in this thread
    time_t t; // time used to generate a random number to pick a jike
    enum serverState ss;  // ss keeps track of where the server is in a conversartion .. see enum for details
    struct regMessage recvmesgReg; // a structure to hold a message recieved which is a REG message
    struct errMessage recvmesgErr; // a strcuture to hold a message reciebed which is a ERR messahe
    char line[1000]; // buf used to build a message which will be sent down to the client
    char *saveptr, *token; // used for tokening the message (the recived message
    int currMesg; //keeps track of currMesg type (REG or ERR)
    int errIndex=-1; // index into the error array .. used to determine current error


    // find out the name and port of the remote host
    error = getnameinfo((struct sockaddr *) &c->addr, c->addr_len, host, 100, port, 10, NI_NUMERICSERV);
   // we provide:
   // the address and its length
   // a buffer to write the host name, and its length
   // a buffer to write the port (as a string), and its length
   // flags, in this case saying that we want the port as a number, not a service name
    if (error != 0) {
        fprintf(stderr, "getnameinfo: %s", gai_strerror(error));
        close(c->fd);
        return NULL;
    }

    fprintf(stdout, "[%s:%s] connection\n", host, port);

    // Set server thread state
    ss = SERVER_START;

    // Send first message
    sprintf(line,"REG|%d|Knock, knock.|",(int)strlen("Knock, knock."));
    // write the message to the client ..
    if (write(c->fd, line, strlen(line)) == -1) {
        fprintf(stderr,"Error while sending to client\n");
        fprintf(stdout, "[%s:%s] Disconection\n", host, port);
        close(c->fd); // close the connection
        free(c); // free it
        return NULL;
    }
    // message sent , so set the server state to KK_REQ_SENT
    ss = SERVER_KK_REQ_SENT;

    // Randomly determine joke index for this thread
    // Intializes random number generator  , and modulo the rand number with totJokes
    srand((unsigned) time(&t));
    jokeIndex =  rand() % totJokes;

    // a new message has started ,
    // we do this to ensure we get the full message even if message is sent in parts
    enum boolean newmesg = true;
    while ((nread = read(c->fd, buf, MAX_MESSAGE_LEN)) > 0) {
        buf[nread] = '\0';
        //printf("[%s:%s] read %d bytes [%s]\n", host, port, nread, buf);
        if (nread <= 0) {
            fprintf(stderr,"zero bytes recieved\nExiting\n");
            break;
        }
        // Stage 1
        // we read multiple times if needed to get the complete message ..
        if (newmesg)  { // new message
            newmesg = false;  // no longer a new message
            if (ss == SERVER_KK_REQ_SENT)
                ss = SERVER_KK_RESP_RECV;
            else if (ss == SERVER_SETUP_REQ_SENT)
                ss = SERVER_SETUP_RESP_RECV;
            else if (ss == SERVER_PUNCH_REQ_SENT)
                ss = SERVER_PUNCH_RESP_RECV;
            else {
                fprintf(stderr,"Server in unknown state\n"); 
                break;
            }

            strcpy(mybuf, buf);  // copty the message recived into mybuf which is trying to create the full expected message
            token = strtok_r(buf,"|",&saveptr); // toeknize the input
            if (token != NULL) {    // we expect atleast message type to have come in
                if (strcmp(token,"REG") == 0 )   // REG message
                    currMesg = REGTYPE;
                else if (strcmp(token,"ERR") == 0 )  // ERR message
                    currMesg = ERRTYPE;
                else {
                    if ((errIndex = getErrCode(ss,FORMAT_BROKEN)) != -1) {
                        sprintf(line,"ERR|%s|",errtype[errIndex]);  // ERR message has just the errocode
                        if (write(c->fd, line, strlen(line)) == -1) {
                            fprintf(stderr,"Error while sending to client\n");
                            break;
                        }
                    }
                    break;  // break out and end the thread
                }
                if (isMessageComplete(currMesg,mybuf) != 0) {
                // check if message is already complete..
                // checks if enough token exist based on message type. REG is 3 tokens and ERR is 2 tokens
                    continue;
                }

            }
            else { // if no token in the first messsage, then its an error, send a ERR message back to client
                // else send error message
                if ((errIndex = getErrCode(ss,FORMAT_BROKEN)) != -1) {
                    sprintf(line,"ERR|%s|",errtype[errIndex]);  // ERR message has just the errocode
                    if (write(c->fd, line, strlen(line)) == -1) {
                        fprintf(stderr,"Error while sending to client\n");
                        break;
                    }
                }
                break;  // break out and end the thread
            }
        }
        else { // message was already started and message type is known, here its about getting the complete message
            strcat(mybuf, buf); // append new read stuff to existing message
            if (isMessageComplete(currMesg,mybuf) != 0) {  // message not complete go back to reading
                continue;
            }
        }
        // *****
        // ******
        // at this point we have the message .. it is complete but may not be correct format
        // next we check the format

        // Stage 2
        // now we tokenize mybuf .. our full message holder
        token = strtok_r(mybuf,"|",&saveptr);
        // for REG message .. message type already known fro above processing
        if (currMesg == REGTYPE) {
            strcpy(recvmesgReg.type,"REG"); // messagetype in recvmesgReg

            token = strtok_r(NULL, "|",&saveptr);  // size of message
            recvmesgReg.messagelen=atoi(token);

            token = strtok_r(NULL, "|",&saveptr);  // message itself
            strcpy(recvmesgReg.message,token);
            // check if the size matches the len provided
            if ((recvmesgReg.messagelen == 0)  || (recvmesgReg.messagelen != strlen(recvmesgReg.message))) {
                // else send error message
                if ((errIndex = getErrCode(ss,LENGTH_INCORRECT)) != -1) {
                    sprintf(line,"ERR|%s|",errtype[errIndex]);  // form the error message
                    // send the error code back to client
                    if (write(c->fd, line, strlen(line)) == -1)
                        fprintf(stderr,"Error while sending to client\n");
                }
                break; // exit out if the length is problem
            }
            token = strtok_r(NULL, "|",&saveptr);
        }       
        else if (currMesg == ERRTYPE) { // ERR messae
            strcpy(recvmesgErr.type,"ERR"); // set message type

            token = strtok_r(NULL, "|",&saveptr);  // sabe the error code
            strcpy(recvmesgErr.errType,token);

            token = strtok_r(NULL, "|",&saveptr);
        }

        // Stage 3
        // if reached until here , it means message is complete as well as in correct format
        // Content could still be incorrect , we check that as part of message processing
        if (currMesg == REGTYPE) {
            if (ss ==  SERVER_KK_RESP_RECV  ) { // if server state is KK RESP RECV
                // check if message is "Who's there?"
                if (strcmp(recvmesgReg.message,"Who's there?") == 0) {
                    // if true , send setup  message using the appropriate randomely selected joke
                    // .. first form the message in line[] and then send it
                    sprintf(line,"REG|%d|%s|",(int)strlen(myjokes[jokeIndex].setup), myjokes[jokeIndex].setup);
                    // send to client
                    if (write(c->fd, line, strlen(line)) == -1) {
                        fprintf(stderr,"Error while sending to client\n");
                        break;
                    }
                    ss = SERVER_SETUP_REQ_SENT; // set server state to SETUP sent
                }
                else {
                    // else if message is not as expected (Who's there?) , send error message  and break to exit
                    if ((errIndex = getErrCode(ss,CONTENT_NOT_CORRECT)) != -1) {
                        sprintf(line,"ERR|%s|",errtype[errIndex]);  // for error code to send
                       // send to client
                        if (write(c->fd, line, strlen(line)) == -1) {
                            fprintf(stderr,"Error while sending to client\n");
                            break;
                        }
                    }
                    break;
                }
            }
            else if (ss ==  SERVER_SETUP_RESP_RECV  ) { // if server state is SETUP RESP RECVT , we expect according response
                // check if message from client is is "<setup message sent>,who?"
                // form our check string in the form "<setup line>,who?"
                char checkSetup[MAX_MESSAGE_LEN+8]; // array to compare.. note the size is longer to acomodate  ".,who?"
                strcpy(checkSetup,myjokes[jokeIndex].setup);
                checkSetup[strlen(checkSetup)-1] = '\0'; // get rid of the "."
                strcat(checkSetup,",who?");
                // compare message recieved with our check string
                if (strcmp(recvmesgReg.message,checkSetup) == 0 ) {
                    // if correct format , send punch  line
                    sprintf(line,"REG|%d|%s|",(int)strlen(myjokes[jokeIndex].punch), myjokes[jokeIndex].punch);
                    if (write(c->fd, line, strlen(line)) == -1) {
                        fprintf(stderr,"Error while sending to client\n");
                        break;
                    }
                    ss = SERVER_PUNCH_REQ_SENT; // set server state to PUNCH SENT
                }
                else {
                    // else send error code and break to exit
                    if ((errIndex = getErrCode(ss,CONTENT_NOT_CORRECT)) != -1) {
                        sprintf(line,"ERR|%s|",errtype[errIndex]);
                        if (write(c->fd, line, strlen(line)) == -1) {
                            fprintf(stderr,"Error while sending to client\n");
                            break;
                        }
                    }
                    break;
                }
            }
            else if (ss ==  SERVER_PUNCH_RESP_RECV  ) {
                fprintf(stdout,"%s\n",recvmesgReg.message);  // just print the A/D/S message sent, no processing done
                fflush(stdout);

                break;  // Done with the conversation , so we breakk  and finally exit the thread

            }
            else {
                fprintf(stderr,"Server in unknown state\n");  // this is unlikely to be excuted since we are ones who always update the serverState
                break;
            }
        }
        else if (currMesg == ERRTYPE) { // Handling Err message recieved by Server is simple.. we just print and exit
            // check if valid err type
            if ((errIndex = getErrIndex(recvmesgErr.errType)) != -1) {
                fprintf(stdout,"%s\n",errmesg[errIndex]); // print the correspnding error message to stdout
                break;
            }
            else {
                fprintf(stderr,"Unknown error message recieved\n");
                break;
            }
        }
        currMesg = 3;
        newmesg = true; // next message read will be new one. so we set newmessage back before we forget

    }


    fprintf(stdout, "[%s:%s] Disconection\n", host, port);
    close(c->fd); // close the connection
    free(c); // free it
    return NULL;
}

/***
    function getErrIndex
    to get ErrIndex , so we get correct errocode. helper functiob
***/
int getErrIndex(char *errTypeStr) {

    for (int i = 0; i<MAX_ERR_TYPES; i++) {
        if (strcmp(errTypeStr,errtype[i]) == 0) {
            return i;
        }
    }
    return -1;
}
/***
    function isMessageComplete
    checks if a REG or ERR message is complete. completion is correct number of tokens
    3 for REG and 2 for ERR
    returns 0 if message is complete. 1 if not
***/

int isMessageComplete(int currMesg, char* buf) {

    char *saveptr, *token;
    int count = 0;
    char mybuf[MAX_MESSAGE_LEN+1];

    strcpy(mybuf,buf);
    token = strtok_r(mybuf,"|",&saveptr);
    while (1) {
        if (token != NULL)
            count++;
        else
            break;
        token = strtok_r(NULL,"|",&saveptr);
    }
    //printf("1:%s\n",buf);
    //printf("messageType:%d, tokenCount=%d\n", (int) currMesg, count);
    // final check   .. a REG message should have 3 tokens and ERR message will have 2
    if ((currMesg == REGTYPE && count == 3) || (currMesg == ERRTYPE && count == 2))
        return 0;
    else
        return 1;
}


/***
    function getErrCode
    returns the correct error code based on crrent serverState and processing problem
***/
int getErrCode (enum serverState ss, enum errorConditions  errCond) {
    if (ss == SERVER_KK_REQ_SENT && errCond == CONTENT_NOT_CORRECT) {
        return (int) M0CT ;
    }
    if (ss == SERVER_KK_REQ_SENT && errCond == LENGTH_INCORRECT) {
        return (int) M0LN ;
    }
    if (ss ==  SERVER_KK_REQ_SENT && errCond == FORMAT_BROKEN) {
        return (int) M0FT ;
    }
    if ((ss == SERVER_KK_RESP_RECV) && errCond == CONTENT_NOT_CORRECT) {
        return (int) M1CT ;
    }
    if ((ss == SERVER_KK_RESP_RECV) && errCond == LENGTH_INCORRECT) {
        return (int) M1LN ;
    }
    if ((ss == SERVER_KK_RESP_RECV) && errCond == FORMAT_BROKEN) {
        return (int) M1FT ;
    }
    if ((ss == SERVER_SETUP_REQ_SENT) && errCond == CONTENT_NOT_CORRECT) {
        return (int) M2CT ;
    }
    if ((ss == SERVER_SETUP_REQ_SENT) && errCond == LENGTH_INCORRECT) {
        return (int) M2LN ;
    }
    if ((ss == SERVER_SETUP_REQ_SENT) && errCond == FORMAT_BROKEN) {
        return (int) M2FT ;
    }
    if ((ss == SERVER_SETUP_RESP_RECV) && errCond == CONTENT_NOT_CORRECT) {
        return (int) M3CT ;
    }
    if ((ss == SERVER_SETUP_RESP_RECV) && errCond == LENGTH_INCORRECT) {
        return (int) M3LN ;
    }
    if ((ss == SERVER_SETUP_RESP_RECV) && errCond == FORMAT_BROKEN) {
        return (int) M3FT ;
    }
    if ((ss == SERVER_PUNCH_REQ_SENT) && errCond == CONTENT_NOT_CORRECT) {
        return (int) M4CT ;
    }
    if ((ss == SERVER_PUNCH_REQ_SENT) && errCond == LENGTH_INCORRECT) {
        return (int) M4LN ;
    }
    if ((ss == SERVER_PUNCH_REQ_SENT) && errCond == FORMAT_BROKEN) {
        return (int) M4FT ;
    }
    if ((ss == SERVER_PUNCH_RESP_RECV) && errCond == CONTENT_NOT_CORRECT) {
        return (int) M5CT ;
    }
    if ((ss == SERVER_PUNCH_RESP_RECV) && errCond == LENGTH_INCORRECT) {
        return (int) M5LN ;
    }
    if ((ss == SERVER_PUNCH_RESP_RECV) && errCond == FORMAT_BROKEN) {
        return (int) M5FT ;
    }
    return 0;
}
