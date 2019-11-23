import java.util.*; 
class Main {

    public static LinkedList<String> tree(int n) {
        if (n == 1) {
            LinkedList<String> to_return = new LinkedList();
            to_return.add("(.)");
            return to_return;
        }

        LinkedList<String> strings = new LinkedList(); 
        LinkedList<String> prev_strings = tree(n - 1);

        for(int i = 0; i < prev_strings.size(); i++) {
            strings.add("(" + prev_strings.get(i) + ".)");
            strings.add("(." + prev_strings.get(i) + ")");
        }
        return strings;
    }
    public static void main(String args[]) {
        System.out.print("Enter the number of nodes: ");
        Scanner s = new Scanner(System.in);
        tree(s.nextInt()).forEach(System.out::println);
        s.close();
    }
}

