with Ada.Containers; use Ada.Containers;
with Ada.Containers.Indefinite_Vectors;

with Ada.Text_IO; use Ada.Text_IO;

with Ada.Command_Line;
procedure Tree is
         X: Positive := Positive'Value(Ada.Command_Line.Argument(1));
        package String_Vectors is new Ada.Containers.Indefinite_Vectors
         (Index_Type   => Natural,
          Element_Type => String);

        function Tree(N: Integer) return String_Vectors.Vector is
        Strings : String_Vectors.Vector;
        begin
            if N = 1 then
                Strings.Append("(.)");
                return Strings;
            end if;
            for T in Tree(N - 1).Iterate loop
                Strings.Append("(" & Natural'Image(String_Vectors.To_Index(T)) & ".)");
                Strings.Append("(." & Natural'Image(String_Vectors.To_Index(T)) & ")");
            end loop;
        return Strings;
        end Tree;


begin
   for Item of Tree(X) loop
      Ada.Text_IO.Put_Line(Item);
   end loop;
end;
