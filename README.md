# SecurID

SecurID is a library for authenticating with an RSA SecurID ACE
Authentication Server.

RSA SecurID is a two-factor authentication scheme that uses a PIN
and a tokencode to authenticate a user.  The tokencode is generated
by a hardware or software token and changes at regular timed intervals.

## Requirements

Installing the SecurID gem requires installation of the proper ACE
authentication library for your system.  The library may be obtained
from RSA.

## Installation

    $ gem build securid.gemspec
    $ gem install securid-X.X.gem

## Usage

    require 'rubygems'
    require 'securid'

    success = RSA::SecurID.authenticate(username, passcode)

The `authenticate` class method accepts a username and a passcode and
returns true or false to indicate success or failure.

The passcode is simply the concatenation of the user's PIN and their
current tokencode.

### Errors

Any unexpected problems during the authentication process will raise an
`RSA::SecurID::SecurIDError`.
