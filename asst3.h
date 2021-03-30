/*
Student Name: Khushi Bhat
*/


// Max length of a message, 110 is just choice used,can be anything
#define MAX_MESSAGE_LEN 110
//Max length of an error type, 4  is just a choice used, can be anything
#define MAX_ERRTYPE_LEN 4

// struct to hold jokes setup and punch line
struct joke {
    char *setup;
    char *punch;
};

// an array of jokes we read from a file of setup and punch line, we are allowing only 100 jokes but can be changed
struct joke myjokes[100] ; // upto 100 jokes allowed
//actual number of jokes in the session
int totJokes;
// structure of an ERR message
struct errMessage {
    char type[4]; // Will be ERR always
    char errType[MAX_ERRTYPE_LEN+1]; // The error code
};
// structure of a REG  message
struct regMessage {
    char type[4]; // Will be REG always
    int messagelen;  // length of the message
    char message[MAX_MESSAGE_LEN+1]; // message
};

// the argument we will pass to the connection-handler threads
struct connection {
    struct sockaddr_storage addr;   // address
    socklen_t addr_len;  // address length
    int fd;  // fils descriptor for the socket
};

enum boolean {false,true}; // enum for boolean
enum errorConditions {CONTENT_NOT_CORRECT,FORMAT_BROKEN, LENGTH_INCORRECT};  // enum for the possible erro conditions
// enum for Server state .. keeps track of where the server is in the flow
enum serverState { SERVER_START , SERVER_KK_REQ_SENT , SERVER_KK_RESP_RECV , SERVER_SETUP_REQ_SENT , SERVER_SETUP_RESP_RECV , SERVER_PUNCH_REQ_SENT , SERVER_PUNCH_RESP_RECV , SERVER_SUCCESS_END };

// all function definitions
int getJokes(char *file);
int validateInput(char **input);
int server(char *port);
void *joker(void *arg);
int getErrIndex(char *errType);
int isMessageComplete(int currMesg, char* buf);
int getErrCode (enum serverState ss, enum errorConditions errCond) ;




#define REGTYPE 0
#define ERRTYPE 1



//All the predefined Error codes and error messages.
#define MAX_ERR_TYPES 18
enum errTypeCode { M0CT, M0LN, M0FT, M1CT, M1LN, M1FT, M2CT, M2LN, M2FT, M3CT, M3LN, M3FT, M4CT, M4LN, M4FT, M5CT, M5LN, M5FT};
char errtype[MAX_ERR_TYPES][5] = { "M0CT", "M0LN", "M0FT", "M1CT", "M1LN", "M1FT", "M2CT", "M2LN", "M2FT", "M3CT", "M3LN", "M3FT", "M4CT", "M4LN", "M4FT", "M5CT", "M5LN", "M5FT"};

char errmesg[MAX_ERR_TYPES][MAX_MESSAGE_LEN+1] = { "message 0 content was not correct (i.e. should be \"Knock, knock.\")", "message 0 length value was incorrect (i.e. should be 13 characters long)", "message 0 format was broken (did not include a message type, missing or too many '|')", "message 1 content was not correct (i.e. should be “Who's there?”)", "message 1 length value was incorrect (i.e. should be 12 characters long)", "message 1 format was broken (did not include a message type, missing or too many '|')", "message 3 content was not correct (i.e. missing punctuation)", "message 2 length value was incorrect (i.e. should be the length of the message)", "message 2 format was broken (did not include a message type, missing or too many '|')", "message 3 content was not correct (i.e. should contain message 2 with “, who?” tacked on)", "message 3 length value was incorrect (i.e. should be M2 length plus six)", "message 3 format was broken (did not include a message type, m
 issing or too many '|')", "message 4 content was not correct (i.e. missing punctuation)", "message 4 length value was incorrect (i.e. should be the length of the message)", "message 4 format was broken (did not include a message type, missing or too many '|')", "message 5 content was not correct (i.e. missing punctuation)", "message 5 length value was incorrect (i.e. should be the length of the message)", "message 5 format was broken (did not include a message type, missing or too many '|')"};