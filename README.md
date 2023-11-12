# UpsServer
Server UPS semestrálka C++ ZS2023


## TCP accept thread
1) Accept incoming TCP connection (ServerSocketBase)
2) Passes client File descriptor to _newClientConnectedHandler (ServerSocketBase)
3) File descriptor is received in _newClientConnectedHandler (TcpGameServer)
4) File descriptor is passed via ~~PIPE?~~ to Idle thread

## Idle thread
1) Receives new client FD from TcpGameServer -> Constructs wrapper around it
2) Sends back CONNECTION OK to client
3) Waits until client sends back his name
4) Checks if client is reconnecting (his name is in an active game)
    
    If client is reconnecting then add him back into the game and send REJOINED GAME OK

    Else continue with next step
5) Send client NAME OK
6) Wait for client to send either LIST GAMES, JOIN GAME or QUIT 