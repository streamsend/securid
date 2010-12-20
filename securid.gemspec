Gem::Specification.new do |s|
  s.name = "securid"
  s.version = "0.1"

  s.authors = ["Ian Lesperance"]
  s.date = "2010-06-10"
  s.description = "A library for authenticating with an RSA SecurID ACE Authentication Server"
  s.email = "ilesperance@ezpublishing.com"
  s.extensions = ["ext/securid/extconf.rb"]
  s.files = ["ext/securid/securid.c", "ext/securid/extconf.rb"]
  s.require_paths = ["lib"]
  s.summary = "A library for authenticating with an RSA SecurID ACE Authentication Server"
  s.homepage = "http://github.com/ezpub/securid"
end
