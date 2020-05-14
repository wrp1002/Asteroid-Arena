# Asteroid Arena
This is a version of Asteroids that is made to be multiplayer. There are two programs included: the client and the server. There are also items, weapons, and an enemy character in this version.

## Getting Started
#### Running the server
To run the server, it must first be configured. Find "Server Config.txt" and change the port to one that you would prefer. Then you are free to run "Asteroid Arena Server.exe" to start up the server.

#### Running the client
 To run the client, start up "Asteroid Arena Client.exe". There will be an option to edit the config file, which you should choose. Edit the config and close it. You can then connect to the server!


## Compiling
If you want to compile this from the included source, you'll need a couple libraries. If you're using Visual Studio, I recommend using the NuGet package manager to install Allegro. However, I couldn't find a source for ENet, so it must be installed manually. 

## Built With

* [C++](https://www.cplusplus.com/) - Programming language
* [Allegro 5](https://www.allegro.cc/) - Game developing library
* [ENet](http://enet.bespin.org/) - Networking library

## Authors

* **[Wesley Paglia](https://github.com/wrp1002)** - *Initial work*


