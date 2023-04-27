//
// Created by Erik Berg on 14.04.23.
//

#include "Bank.h"
#include "../Data/initData.h"
#include "functional"

std::mutex muBank;

////////////////////////////////////////////////////////////////////STATUS messages//////////////////////////////////////////////////////////////

std::string getHttpStatusMessage(int status_code)
{
    switch (status_code)
    {
    case 100:
        return "Continue";
    case 101:
        return "Switching Protocols";
    case 200:
        return "OK";
    case 201:
        return "Created";
    case 202:
        return "Accepted";
    case 203:
        return "Non-Authoritative Information";
    case 204:
        return "No Content";
    case 205:
        return "Reset Content";
    case 206:
        return "Partial Content";
    case 300:
        return "Multiple Choices";
    case 301:
        return "Moved Permanently";
    case 302:
        return "Found";
    case 303:
        return "See Other";
    case 304:
        return "Not Modified";
    case 305:
        return "Use Proxy";
    case 307:
        return "Temporary Redirect";
    case 400:
        return "Bad Request";
    case 401:
        return "Unauthorized";
    case 402:
        return "Payment Required";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 406:
        return "Not Acceptable";
    case 407:
        return "Proxy Authentication Required";
    case 408:
        return "Request Timeout";
    case 409:
        return "Conflict";
    case 410:
        return "Gone";
    case 411:
        return "Length Required";
    case 412:
        return "Precondition Failed";
    case 413:
        return "Request Entity Too Large";
    case 414:
        return "Request-URI Too Long";
    case 415:
        return "Unsupported Media Type";
    case 416:
        return "Requested Range Not Satisfiable";
    case 417:
        return "Expectation Failed";
    case 500:
        return "Internal Server Error";
    case 501:
        return "Not Implemented";
    case 502:
        return "Bad Gateway";
    case 503:
        return "Service Unavailable";
    case 504:
        return "Gateway Timeout";
    case 505:
        return "HTTP Version Not Supported";
    default:
        return "Unknown";
    }
}

////////////////////////////////////////////////////////////////////JSON specific//////////////////////////////////////////////////////////////

std::string generateJsonResponse(Bank bank)
{
    // Create JSON response using RapidJSON
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();

    // Add variables to JSON object
    doc.AddMember("totalValue", bank.getTotalValue(), allocator);
    doc.AddMember("cashReserves", bank.getCashReserves(), allocator);
    doc.AddMember("outstandingLoans", bank.getOutstandingLoans(), allocator);

    // Create an array of stocks
    rapidjson::Value stocks(rapidjson::kArrayType);
    for (auto stock : bank.getPortfolio())
    {
        rapidjson::Value stockObj(rapidjson::kObjectType);
        stockObj.AddMember("acronym", rapidjson::StringRef(stock->getAcronym().c_str()), allocator);
        stockObj.AddMember("price", stock->getPrice(), allocator);
        stockObj.AddMember("amount", stock->getAmount(), allocator);
        stocks.PushBack(stockObj, allocator);
    }
    doc.AddMember("stocks", stocks, allocator);

    // Convert JSON document to string
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);
    return strbuf.GetString();
}

void sendJsonResponse(int client_sock, int status_code, const std::string &response)
{
    // Build HTTP response
    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 " << status_code << " " << getHttpStatusMessage(status_code) << "\r\n";
    response_stream << "Content-Type: application/json\r\n";
    response_stream << "Content-Length: " << response.size() << "\r\n";
    response_stream << "\r\n";
    response_stream << response;

    // Send response to client
    int bytes_sent = send(client_sock, response_stream.str().c_str(), response_stream.str().length(), 0);
    if (bytes_sent < 0)
    {
        std::cerr << "Failed to send response" << std::endl;
    }
}

////////////////////////////////////////////////////////////////////TEXT response//////////////////////////////////////////////////////////////

void sendTextResponse(int client_sock, int status_code, const std::string &message)
{
    // Build HTTP response
    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 " << status_code << " " << getHttpStatusMessage(status_code) << "\r\n";
    response_stream << "Content-Type: text/plain\r\n";
    response_stream << "Content-Length: " << message.size() << "\r\n";
    response_stream << "\r\n";
    response_stream << message;

    // Send response to client
    int bytes_sent = send(client_sock, response_stream.str().c_str(), response_stream.str().length(), 0);
    if (bytes_sent < 0)
    {
        std::cerr << "Failed to send response" << std::endl;
    }
}

////////////////////////////////////////////////////////////////////HTTP Handler//////////////////////////////////////////////////////////////

void handleOtherRequests(int client_sock)
{
    // Build HTTP response
    std::string message = "Die angeforderte Methode wird für diese Ressource nicht unterstützt.\n Erlaubte Methoden: GET, POST.";
    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 " << 405 << " " << getHttpStatusMessage(405) << "\r\n";
    response_stream << "Allow: GET, POST\r\n";
    response_stream << "Content-Type: text/plain\r\n";
    response_stream << "Content-Length: " << message.size() << "\r\n";
    response_stream << "\r\n";
    response_stream << message;

    // Send response to client
    int bytes_sent = send(client_sock, response_stream.str().c_str(), response_stream.str().length(), 0);
    if (bytes_sent < 0)
    {
        std::cerr << "Failed to send response" << std::endl;
    }
}

void handleGetRequest(int client_sock, const std::string &path, Bank bank)
{
    // Check if path is correct
    if (path != "/bankInfo")
    {
        sendJsonResponse(client_sock, 404, "");
        return;
    }
    sendJsonResponse(client_sock, 200, generateJsonResponse(bank));
}

void handlePostRequest(int client_sock, const std::string &path, const std::string &data, Bank bank)
{
    // Determine the endpoint
    if (path.find("/stocks/") == 0)
    {
        // Endpoint to update the quantity of a stock
        std::string acronym = path.substr(strlen("/stocks/"));
        int amount;
        rapidjson::Document doc;
        doc.Parse(data.c_str());
        if (!doc.HasParseError() && doc.HasMember("amount") && doc["amount"].IsInt())
        {
            amount = doc["amount"].GetInt();
            muBank.lock();
            bank.updateStockAmount(acronym, (unsigned int)amount);
            muBank.unlock();
            std::string response = "Amount of stock " + acronym + " updated to " + std::to_string(amount) + ".";
            sendTextResponse(client_sock, 200, response);
        }
        else
        {
            std::string response = "Invalid request data";
            sendTextResponse(client_sock, 400, response);
        }
    }
    else if (path == "/cash")
    {
        // Endpoint to update cash reserves
        int cashReserves;
        rapidjson::Document doc;
        doc.Parse(data.c_str());
        if (!doc.HasParseError() && doc.HasMember("cashReserves") && doc["cashReserves"].IsInt())
        {
            cashReserves = doc["cashReserves"].GetInt();
            muBank.lock();
            bank.updateCashreserves(cashReserves);
            muBank.unlock();
            std::string response = "Cash reserves changed to " + std::to_string(cashReserves) + "€.";
            sendTextResponse(client_sock, 200, response);
        }
        else
        {
            std::string response = "Invalid request data";
            sendTextResponse(client_sock, 400, response);
        }
    }
    else if (path == "/credit")
    {
        // Endpoint to update outstanding loans
        int outstandingLoans;
        rapidjson::Document doc;
        doc.Parse(data.c_str());
        if (!doc.HasParseError() && doc.HasMember("outstandingLoans") && doc["outstandingLoans"].IsInt())
        {
            outstandingLoans = doc["outstandingLoans"].GetInt();
            muBank.lock();
            bank.updateOutstandingLoans(outstandingLoans);
            muBank.unlock();
            std::string response = "Outstanding loans changed to " + std::to_string(outstandingLoans) + "€.";
            sendJsonResponse(client_sock, 200, response);
        }
        else
        {
            std::string response = "Invalid request data";
            sendTextResponse(client_sock, 400, response);
        }
    }
    else
    {
        std::string response = "Invalid request path";
        sendTextResponse(client_sock, 404, response);
    }
}

////////////////////////////////////////////////////////////////////HTTP interface//////////////////////////////////////////////////////////////

void interface(Bank bank)
{
    std::cout << "Start bankInterface" << std::endl;
    // Create a TCP socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        std::cerr << "Failed to create socket" << std::endl;
        exit(1);
    }

    // Set up the address and port to bind to
    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(TCP_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address and port
    int status = bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status < 0)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        exit(1);
    }

    // Listen for incoming connections
    status = listen(server_sock, 5);
    if (status < 0)
    {
        std::cerr << "Failed to listen for connections" << std::endl;
        exit(1);
    }

    // Accept incoming connections and send a response
    while (true)
    {
        std::cout << "Waiting for connection..." << std::endl;
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_address_size);
        if (client_sock < 0)
        {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        // Receive data from the client
        char buffer[1024];
        int received_bytes = recv(client_sock, buffer, sizeof(buffer), 0);
        if (received_bytes < 0)
        {
            std::cerr << "Failed to receive data" << std::endl;
            close(client_sock);
            continue;
        }

        // Parse the request
        std::istringstream request_stream(buffer);
        std::string request_line;
        std::getline(request_stream, request_line);
        std::istringstream request_line_stream(request_line);
        std::string http_method, path, http_version;
        request_line_stream >> http_method >> path >> http_version;

        // Determine the request method
        if (http_method == "GET")
        {
            // Handle GET request
            handleGetRequest(client_sock, path, bank);
        }
        else if (http_method == "POST")
        {
            // Handle POST request
            // Extract data from the request body
            std::string data = request_stream.str().substr(request_stream.tellg());
            handlePostRequest(client_sock, path, data, bank);
        }
        else
        {
            // Handle other request methods
            std::cerr << "Unsupported HTTP method: " << http_method << std::endl;
            handleOtherRequests(client_sock);
        }

        // Close the client socket
        close(client_sock);
    }

    // Close the server socket
    close(server_sock);
}

////////////////////////////////////////////////////////////////////UDP specific//////////////////////////////////////////////////////////////
std::string createRegisterMessage(Bank bank)
{
    // Set first attribute of register message as bankname (which is equal to hostname)
    std::string regMsg = bank.getName() + " ";

    // Set second argument as ttype (sub- or desubscribe // "sub" "desub")
    regMsg += "sub ";

    // Set third attribute of register message as number of stocks in portfolio
    regMsg += std::to_string(bank.getPortfolio().size()) + " ";

    // Add all stock acronyms of portfolio to the register message
    for (auto stock : bank.getPortfolio())
    {
        regMsg += stock->getAcronym() + " ";
    }

    std::cout << "Registration message: " << regMsg << std::endl;
    return regMsg;
}

void registerToStockMarket(Bank bank)
{
    std::cout << "Start registration process for " << bank.getName() << std::endl;

    int sockfd;
    // Creating socket file descriptor for stock market
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cerr << "Error: socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in stockMarketAddr;

    const char *hostname = "stockMarket";
    struct hostent *stockMarket = gethostbyname(hostname);
    if (stockMarket == NULL)
    {
        std::cerr << "Error: could not resolve hostname" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Filling stockMarket information
    memset(&stockMarketAddr, 0, sizeof(stockMarketAddr));
    stockMarketAddr.sin_family = AF_INET;
    stockMarketAddr.sin_addr = *((struct in_addr *)stockMarket->h_addr);
    stockMarketAddr.sin_port = htons(UDP_PORT);

    std::cout << "Registering to Stock Market with IP: " << inet_ntoa(stockMarketAddr.sin_addr) << std::endl;

    std::string regMsg = createRegisterMessage(bank);
    // Send a registration message with hostname and all stock acronyms to the stockMarket
    sendto(sockfd, regMsg.c_str(), regMsg.length(), 0, (struct sockaddr *)&stockMarketAddr,
           sizeof(stockMarketAddr));

    std::cout << "Registration sent" << std::endl;
}

void sendMessageToStockMarket(std::string message)
{
    std::cout << "Sending Message: " << message << std::endl;

    int sockfd;
    // Creating socket file descriptor for stock market
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cerr << "Error: socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in stockMarketAddr;

    const char *hostname = "stockMarket";
    struct hostent *stockMarket = gethostbyname(hostname);
    if (stockMarket == NULL)
    {
        std::cerr << "Error: could not resolve hostname" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Filling stockMarket information
    memset(&stockMarketAddr, 0, sizeof(stockMarketAddr));
    stockMarketAddr.sin_family = AF_INET;
    stockMarketAddr.sin_addr = *((struct in_addr *)stockMarket->h_addr);
    stockMarketAddr.sin_port = htons(UDP_PORT);

    // Send a registration message with hostname and all stock acronyms to the stockMarket
    sendto(sockfd, message.c_str(), message.length(), 0, (struct sockaddr *)&stockMarketAddr,
           sizeof(stockMarketAddr));
}

void receiveMessage(Bank bank)
{
    std::cout << "Start receiveMessage" << std::endl;

    // Creating socket file descriptor for bank
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cerr << "Error: socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in bankAddr;
    // Filling bank information
    memset(&bankAddr, 0, sizeof(bankAddr));
    bankAddr.sin_family = AF_INET;
    bankAddr.sin_addr.s_addr = INADDR_ANY;
    bankAddr.sin_port = htons(UDP_PORT);

    // Bind the socket with the bank address
    if (bind(sockfd, (const struct sockaddr *)&bankAddr,
             sizeof(bankAddr)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << bank.getName() << " ready for receiving messages" << std::endl;

    while (true)
    {
        // std::cout << "Before receive" << std::endl;
        //  wait for an incoming message
        std::string message;
        message.resize(1024); // allocate space for the received message
        socklen_t addrlen = sizeof(bankAddr);
        int nbytes = recvfrom(sockfd, &message[0], message.size(), 0, (struct sockaddr *)&bankAddr,
                              &addrlen);
        if (nbytes < 0)
        {
            std::cerr << "Error receiving message" << std::endl;
            break;
        }
        // std::cout << "After receive" << std::endl;
        // split the message into its parts
        std::istringstream iss(message);
        std::string acronym, needAck;
        unsigned int price, amount;
        iss >> acronym >> price >> amount >> needAck;

        // print the received message parts and source address
        std::cout << bank.getName() << " received " << nbytes << " bytes from "
                  << inet_ntoa(bankAddr.sin_addr) << std::endl;
        std::cout << "TRANSACTION ->\tAcronym: " << acronym << "\t";
        std::cout << "Price: " << price << "\t";
        std::cout << "Amount: " << amount << std::endl;

        muBank.lock();
        bank.updateStock(acronym, price);
        muBank.unlock();

        if (needAck == "true")
        {
            std::string ackMessage = bank.getName() + " ACK ";
            sendMessageToStockMarket(ackMessage);
        }
        // std::cout << "INSIDE receiveMessage()" << std::endl;
        // std::cout << "INSIDE receiveMessage()" << std::endl;
    }
    // std::cout << "End receiveMessage" << std::endl;
}

////////////////////////////////////////////////////////////////////RUN bank//////////////////////////////////////////////////////////////
int main()
{
    std::string containerName = std::getenv("CONTAINER_NAME");
    std::string myIP = std::getenv("MY_IPV4_ADDRESS");
    std::hash<std::string> nameHash;
    unsigned int seed = nameHash(containerName);
    srand(seed);

    Bank bank(fillPortfolio(), containerName, myIP);

    registerToStockMarket(bank);
    std::thread interfaceThread([&bank]()
                                { interface(bank); });
    std::thread receiveMessageThread([&bank]()
                                     { receiveMessage(bank); });

    interfaceThread.join();
    receiveMessageThread.join();

    return 0;
};