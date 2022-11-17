# TODO

### Ethernet physycal disconnection protection
- [ ] Protect the code from beeing stuck because of a physicall deconnection. 
  - [ ] check linkStatus in main loop.
  - [ ] reconnect if link lost in the main loop.
  - [ ] check linkStatus while reading or writing operation occured.
  - [ ] reconnect if link lost while reading or writing operation occured.

### Handling bad chars from Serial
- [ ] Check if they are bad chars (especially for '"') as we send back to the network client JSON formatted response.

### Better JSON format response
- [ ] Json object is not standard (depending or errors). It will be better to implement something more generic to help clients with parsing response.
- [ ] Maybe use a JSON lib to be more readable in the code.

### Clean The Code
- [ ] Make some extenal libs or class.
- [ ] Decrease the number of global variables.
- [ ] More comments.
- [ ] Optimize the code.