# Bugs #
  * When the host leaves its doing a terrible job of handling who gets to be the new host.
  * If you throw an exception in the constructor of a managed object it will cause an access violation when cleaning up that object
  * When a user has 2 players in the same game its really difficult to tell which is which on the directory server. This could lead to exploits in stealing host privileges.
  * Configuration is pretty poorly implemented
  * Console logging is duct taped into the Rolling file appender, and the rolling file appender never rolls.
  * I would suspect theres alot of mutex contention on the logging.


# Features I want #
  * HTTPS for directory clients, TLS for game clients
  * Better feedback mechanism for thread events
  * UI/3D abstraction API
  * Scripting
  * Content management tools (editors?)

I guess, ideally, for me would be coding most things in c++ but the rest in JavaScript or some other scripting language. Without having to update all the tools every time you change the game mechanics at the code level unless the mechanic change warrants some new functionality or you've changed some existing piece pretty significantly.