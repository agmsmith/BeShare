
BeShare Robot Skeleton Code v1.21
Jeremy Friesner (jaf@lcsaudio.com)
1/22/02

This is just some simple code that can be used as the basis
for a "bot" (computerized user) for BeShare.  It is here for
you to play with if you like building bots.

-Jeremy


Make sure that you read the Bot Guidelines document as well, it
contains importatnt information.

-Alan, aka void (alan@cgsoftware.com)

HOW TO BUILD
------------

- Download MUSCLE (2.00 or higher) and unpack it so that the 'muscle'
  directory is in the directory above this one.
- type 'make' to build the sharebot.


HOW TO MODIFY
-------------

This is some skeleton code for making your own groovy BeShare bot.
Look in sharebot.cpp for functions that you can add to... they
include:

ReceivedChatMessage()       (called when someone sends some chat text)
ReceivedPrivateMessage()    (called when someone sent your bot a /msg)
UserLoggedInOrChangedName() (called when... well, you can guess)
UserLoggedOut()             (called when a user logs out)

Functions your code is encouraged to call:

SendPublicMessage()         (send out a public chat text message)
SendPrivateMessage()        (send a /msg to a particular user or users)

This code is public domain, do what you like with it (but please 
be considerate and don't run bots that are annoying the other BeShare 
users!  If you want to test your 'bot, it's better to do it on
your own MUSCLE server (i.e. have your bot connect to 'localhost' then
to do it on e.g. beshare.bentonrea.com.  Thanks!)


