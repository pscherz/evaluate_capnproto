@0xd04c5b614ab1756f;

struct Person {
  name @0 :Text;
  id @1 :Int32;
  email @2 :Text;
}

struct Addressbook {
  people @0 :List(Person);
}
