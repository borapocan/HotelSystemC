# Hotel System C

## Description

This project is going to have 2 software. the first is server and the second is client.
both software are going to design in C. The main os for testing is windows,

There are 2 user types in this system. Admin or Normal user

About server software:

We have 3 json files in Config directory file next to server.c file and we need to load these json file and decode them.

base.json:

{
    "hostName":"127.0.0.1",
    "commandChannelPort": 8000
  }
  

UsersInfo.json

{
    "users": 
    [
      {
        "id":0,
        "user":"Admin",
        "password":"admin",
        "admin":"true"
      },
      {
        "id":1,
        "user":"Morad",
        "password":"1234",
        "admin":"false",
        "purse":"1000",
        "phoneNumber":"00945692356",
        "address":"Canada, Montral"
      },
      {
        "id":2,
        "user":"Jasem",
        "password":"4321",
        "admin":"false",
        "purse":"200",
        "phoneNumber":"00125692356",
        "address":"United States, Newyork"
      },
      {
        "id":3,
        "user":"Pedram",
        "password":"6789",
        "admin":"false",
        "purse":"10",
        "phoneNumber":"00989112659874",
        "address":"Iran, Rasht"
      }
    ]
}

RoomsInfo.json

{
    "rooms": 
    [
      {
        "number":"101",
        "status":0,
        "price":100,
        "maxCapacity":2,
        "capacity":2,
        "users":
        [
        ]
      },
      {
        "number":"102",
        "status":0,
        "price":73,
        "maxCapacity":3,
        "capacity":2,
        "users":
        [
            {
                "id":1,
                "numOfBeds":1,
                "reserveDate":"25-02-2023",
                "checkoutDate":"27-02-2023"
            }
        ]
      },
      {
        "number":"201",
        "status":1,
        "price":100,
        "maxCapacity":2,
        "capacity":0,
        "users":
        [
            {
                "id":2,
                "numOfBeds":2,
                "reserveDate":"25-02-2023",
                "checkoutDate":"27-02-2023"
            }
        ]
      },
      {
        "number":"301",
        "status":0,
        "price":100,
        "maxCapacity":1,
        "capacity":0,
        "users":
        [
            {
                "id":2,
                "numOfBeds":1,
                "reserveDate":"27-02-2023",
                "checkoutDate":"28-02-2023"
            },
            {
                "id":3,
                "numOfBeds":1,
                "reserveDate":"25-02-2023",
                "checkoutDate":"26-02-2023"
            }
        ]
      }
    ]
  }



They are 3 config files. base.json is for server configuration. UsersInfo.json is for users information and RoomsInfo.json is for rooms information.

In the initial state of server software, we need to load these json files and decode them. After that, we need to create a socket and listen to the port that is defined in base.json file. After that, we need to accept the connection from client softwares. After that, we need to receive the command from client software and answer these.

About the client software:

We need to create a socket and connect to the server. After that, we need to send the command to the server and receive the answer from the server.

First we need to make sure the server is available. If the server is not available, we need to show the error message and exit the program.

When client software opens, There are 2 commands.
If user type invalid command, we need to show the error message and re-ask the user to enter a command.

1- signin <username> <password>

If user type wrong or invalid argument, we will show error 430. If user type correct user and pass we will show message 230. If user and password were not correct we will show error 431.

2- signup <username> (and then we need to ask for password, purse, phone, and address)

If the username already exists show message 451. otherwise print message 311 and after that we will ask for more details about the user.
The first is password, next is purse and phone and address.
If user type wrong or invalid value for these values, we need to show error message 503.
but if everything is okay print message 231.

About the sign-in command if user type correct user and pass we need to detect from server if user is admin or normal user. If user is admin, we need to show the admin menu. If user is normal user, we need to show the normal user menu.

So from this step client software will divide into 2 parts. Admin client and normal user client.

Here is the menu:
- 1: View user information
- 2: View all users
- 3: View rooms information
- 4: Booking
- 5: Canceling
- 6: Pass day
- 7: Edit information
- 8: Leaving rooms
- 9: Rooms
- 0: Logout


And ask the user:
```
Command -> <Choise number>
```


If admin or user enter wrong or invalid number, we will show message 503.
Otherwise we will call sub related functino to process and continue the command.



About the first command: View user information

Both user and admin has access to it and can view the information of current user who signed in. after printing the information they will back to the menu.

About the second command: View all users

This command is only for admins. if a normal user enter this command, we will show error 403 and back to the menu.
Here we will print all information of users except the password.

About the third command: View rooms informations

This command will show list of all rooms and status of these rooms. if a room is empty, we will show the room is empty otherwise if the signed user is an admin we will show the list of users who are in the room.

About the fourth command: Booking

This command is for normal user.

They can book a free room by entering start date, end date and number of beds. If the room is not available, we will show error 404. If the room is available, we will show message 230 and show the room number and price.

book <roomNum> <NumOfBeds> <CheckInDate> <CheckOutDate>


Note price of a room is calculated by number of beds and number of days.
The "price" field in roomsInfo.json file is the price of 1 bed for 1 day. If user want to book a room with 2 beds for 2 days, the price will be 2 * 2 * price.


If user not have balance, we will print message 108.
if user enter the command with wrong arguments or fewer arguments, we will print error 503.

If user enter the wrong room number we will print 101.
If the room is full in the range of dates, we will print 109.



About fifth command: Canceling

This command is for normal user. This command is about when a user want to checkout a room sooner than the checkout date.

> cancel <RoomNum> <Num>
The second is number of reservation. What is this? Maybe a people reserved a room for 2 persons and now want to cancel it for one person. So we need to know which reservation we need to cancel. Or maybe a user reserved a room for 5 persons and now want to cancel it for 3 persons.


We will refund half of the price of the room to the user for remaining days. because the user already paid for the room and we can not refund all of the price.
For example if user booked a room for 5 days and now in the second day, he want to checkout. we will refund half of the price of the room for 3 days.



If we cannot find the room, we will print message 101.
If we cannot find any reservation for the room for current signed user, we will print message 102.
 or even when number of beds (reservation) is not correct or user entered a number bigger than the number of beds that he booked for that room, we will print message 102.

If user entered wrong arguments or fewer arguments, we will print error 401.

If everything is okay and we canceled the room print 110.


About the sixth command: Pass day

This command is only for admins. and if a normal user enter this command, we will show error 403 and back to the menu.

Here admin can enter number of days that he want to pass.

For example entered date in the server is 2023-02-25 and admin entered 2. so the date will be 2023-02-27.

About the seventh command: Edit information

This command is for both admin and normal users. and they can view their user signed information and edit it.

> <new password>
> <phone>
> <address>


About the eighth command: Leaving rooms

This command is for both admin and normal users. and they can leave the room that they are in sooner than the checkout date.
