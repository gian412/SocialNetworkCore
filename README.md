# Monitoring system of relationships between elements

---

### The topic

---

- We want to implement a mechanism for monitoring relations between entities (e.g. people) that change over time

- Imagine, for example, a social network, in which new users can register, and existing users can cancel their own account, become “friends” with other users, break the relationship of friendship, etc.

- Relationships between entities are not necessarily symmetrical. For example, Alice may be Bruno’s “friend”, but friendship is not mutual (Bruno is not Alice’s friend)

- More abstractly, the mechanism monitors the following phenomena:

  - A new entity begins to be monitored

  - A monitored entity stops being monitored

  - A new relationship is established between 2 monitored entities

  - A relationship existing between 2 monitored entities ceases to exist

- Each entity has an identifying name (for example “Alice”, “Bruno”, “Carlo”)

- There can be different types of relationships between entities, each identified by a name (for example, “friend_of”, “follows”, “same_age_of”)

- Every relationship has a direction (for example, if Alice is “friend_of” Bruno, the direction of the relationship is from Alice to Bruno, so Bruno is the “receiver” of the relationship), and he is not necessarily symmetrical

- In response to a specific command, the system returns, for each relationship, the entity that “receives” multiple relationships (if there are multiple entities whose number of relationships received is maximum, these are printed in ascending order of identification)

- The application must be optimized to manage a large number of entities and instances of relationships, but generally few (identifying) types of relationships

### The project

---

Standard C language implementation (with libc only) of a program that reads a sequence of commands from standard input, each corresponding to a change in the entities or in the relationships between entities and, when required, produces on standard output, for each type of monitored relationship, the identifier of the entity which is the receiver of the greatest number of instances of that relationship, and the number of relationships the entity receives

### Commands

---

- addent ⟨id_ent⟩

  - Adds an entity identified by “id_ent” to the set of monitored entities; if the entity is already monitored, it does nothing

- delent ⟨id_ent⟩

  - Deletes the entity identified by “id_ent” from the set of monitored entities; delete all relations of which “id_ent” is a part (both as origin and as destination)

- addrel ⟨id_orig⟩ ⟨id_dest⟩ ⟨id_rel⟩

  - Adds a relationship - identified by “id_rel” - between the entities “id_orig” and “id_dest”, where “id_dest” is the receiver of the report. If the relationship between “id_orig” and “id_dest” already exists, or if at least one of the entities is not monitored, it does nothing. The monitoring of the “id_rel” relationship type implicitly begins with the first “addrel” command that affects it.

- delrel ⟨id_ent⟩ id_orig⟩ ⟨id_ent⟩ id_dest⟩ ⟨id_ent⟩ id_rel⟩

  - Deletes the relationship identified by “id_rel” between the entities “id_orig” and “id_dest” (where “id_dest” is the receiver of the relationship); if there is no “id_rel” relationship between “id_orig” and “id_dest” (with “id_dest” as receiver), it does nothing

- report

  - Outputs the list of relations, reporting for each entity with the largest number of incoming relations, such as explained later

- end

  - end of the command sequence

### Notes

---

- Identifiers (both entity and relationship) are always enclosed in "" and can only contain letters (upper or lower case), digits, and the symbols ‘\_’ and ‘-‘

- It's not necessary to check that the received identifiers respect this
  convention, it can be taken for granted

- All identifiers (both of entities and relationships) are case sensitive, so "Alice" and "alice" are different identifiers

- The output of the report command is a sequence made as follows:
  ⟨id_rel1⟩ ⟨id_ent1⟩ ⟨n_rel1⟩; ⟨id_rel2⟩ ⟨id_ent2⟩ ⟨n_rel2⟩; ...

  - The output reports are sorted in ascending order of identifier

  - If for a type of relationship there are more entities that are recipients of the maximum number of relationships,
    these are produced in ascending order of identification, for example:
    ⟨id_rel1⟩ ⟨id_ent1_1⟩ ⟨id_ent1_2⟩ ⟨id_ent1_3⟩ ... ⟨n_rel1⟩;

  - If all the relations with a certain identifier are removed, it does not appear in the followings report command's output

  - If there are no relationships between the entities, the output is _none_ (without quotes)

  - The sort of the identifiers follows the table of ASCII characters, for which the
    following order: - <1 <A <\_ <a

  - The various parts of each command and each output sequence are separated by spaces

  - The end command has no output

### Command sequence example (with I/O)

---

| input                             | output                                                 |
| --------------------------------- | ------------------------------------------------------ |
| addent "Alice"                    |                                                        |
| addent "Bob"                      |                                                        |
| addent "Carl"                     |                                                        |
| addent "Dave"                     |                                                        |
| report                            | none                                                   |
| addrel "Carl" "Bob" "friend_of"   |                                                        |
| report                            | "friend_of" "Bob" 1;                                   |
| addrel "Carl" "Alice" "friend_of" |                                                        |
| report                            | "friend_of" "Alice" "Bob" 1;                           |
| addrel "Alice" "Bob" "friend_of"  |                                                        |
| report                            | "friend_of" "Bob" 2;                                   |
| addrel "Bob" "Dave" "mate_of"     |                                                        |
| report                            | "friend_of" "Bob" 2; "mate_of" "Dave" 1;               |
| delrel "Carl" "Alice" "friend_of" |                                                        |
| report                            | "friend_of" "Bob" 2; "mate_of" "Dave" 1;               |
| addrel "Carl" "Alice" "mate_of"   |                                                        |
| report                            | "friend_of" "Bob" 2; "mate_of" "Alice" "Dave" 1;       |
| addrel "Carl" "Bob" "mate_of"     |                                                        |
| report                            | "friend_of" "Bob" 2; "mate_of" "Alice" "Bob" "Dave" 1; |
| delent "Alice"                    |                                                        |
| report                            | "friend_of" "Bob" 1; "mate_of" "Dave" 1;               |
| end                               |                                                        |
