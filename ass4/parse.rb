#!/usr/bin/ruby
all_dirs = []

all_dwarf = []
dirs = ["."]

# funcs stores the mappings from assembly function names to the instructions.
funcs = Hash.new

dwarf_file = "#{ARGV[0]}.dwf"
diss_file = "#{ARGV[0]}.obj"

dwarf = Hash.new
# iterate the dwarf dump line by line 
IO.foreach(dwarf_file) {|block|
  
  if block =~ /debug_line\[0x[0-9]*\]/
    
    all_dirs.push(dirs)
    all_dwarf.push(dwarf)
    dirs = ["."]
    dwarf = Hash.new
  end  
  # read dwarf dump and store directory mappings in list "dirs"
  if block =~ /include_directories*/
    dir = block.scan(/"(.*)"/).first.first
    dirs.push(dir) 
  end
  
  # store information regarding each instruction in
  # hashmap "dwarf": address -> list containing information about instruction
  if block =~ /^0x*/
    ins = block.split(" ")
    ins[1] = (Integer(ins[1]) - 1).to_s
    dwarf[ins[0].scan(/0x0*(.*)/).first.first] = ins[1..]
    
  end
}

all_dirs.push(dirs)
all_dwarf.push(dwarf)


file_names = [""]
all_file_names = []

# interate the dwarf dump line by line and store file names to file index mappings in the list "file_names"
IO.foreach(dwarf_file).each_cons(5).with_object([]) { |block| 

  if block[0] =~ /debug_line\[0x[0-9]*\]/
    all_file_names.push(file_names)
    file_names = [""]
  end  
  if block[0] =~ /file_names.*/
    file_names.append(block[1..4].map{|e| e.split(':')[1].gsub(/\s/, "").gsub(/"/, "")})
  end

}
all_file_names.push(file_names)

file = File.open(diss_file, "r")
# od means object dump
od = file.read
od_lines = od.split("\n")

od_lines.each_with_index { |line, index|
  # if "line" is the header of an assembly block...
  if line =~ /<.*>:/
    f_name = line.scan(/<(.*)>:/).first.first
    body = []
    i = 1
    # the condition makes sure that we read only until the beginning of the next block
    until od_lines[index + i] =~ /<.*>:/ or index + i > od_lines.length do
      if od_lines[index + i] =~ /^ [a-f 0-9]*:/
        ins = []
        # read the address
        ins.append(od_lines[index + i].scan(/^  [0-9a-f]*/).first.gsub(/\s+/, ""))
        # read hexadecimal representation of the instruction
        ins.append(od_lines[index + i].scan(/^  [0-9a-f]*:\t([0-9a-f ]*)/).first.first)
        # read instruction type
        ins.append(od_lines[index + i].scan(/^  [0-9a-f]*:\t[0-9a-f ]*\t([a-z0-9]*)/).first)

        # if instruction is callq, jae etc. then append the address and the callee
        if od_lines[index+i] =~ / *\tcallq/ or od_lines[index+i] =~ / *\tjae/ or od_lines[index+i] =~ / *\tje/ or od_lines[index+i] =~ / *\tjne/ or od_lines[index+i] =~ / *\tjmp/ or od_lines[index+i] =~ / *\tjbe/ or od_lines[index+i] =~ / *\tjb/ 
          call_add = od_lines[index + i].scan(/^  [0-9a-f]*:\t[0-9a-f ]*\t[a-z0-9]* *([0-9a-f]*)/).first.first
          callee = od_lines[index + i].scan(/^  [0-9a-f]*:\t[0-9a-f ]*\t[a-z0-9]* *[0-9a-f]* <(.*)>/).first
          ins.append([call_add, callee])
        else
          ins.append(od_lines[index + i].scan(/^  [0-9a-f]*:\t[0-9a-f ]*\t[a-z0-9 ]*(.*)/).first)
        end 
        body.append(ins)
      end
      i = i + 1
    end
    funcs[f_name] = body
  end 
}
# all c++ keywords for syntax highlighting. See function "highlight" for its use
$cpp_keywords=[\
"alignas",\
"alignof",\
"and",\
"and_eq",\
"asm",\
"atomic_cancel",\
"atomic_commit",\
"atomic_noexcept",\
"auto",\
"bitand",\
"bitor",\
"bool",\
"break",\
"case",\
"catch",\
"char",\
"char8_t",\
"char16_t",\
"char32_t",\
"compl",\
"concept",\
"const",\
"consteval",\
"constexpr",\
"constinit",\
"const_cast",\
"continue",\
"co_await",\
"co_return",\
"co_yield",\
"decltype",\
"default",\
"delete",\
"do",\
"double",\
"dynamic_cast",\
"else",\
"enum",\
"explicit",\
"export",\
"extern",\
"false",\
"float",\
"for",\
"friend",\
"goto",\
"if",\
"inline",\
"int",\
"long",\
"mutable",\
"namespace",\
"new",\
"noexcept",\
"not",\
"not_eq",\
"nullptr",\
"operator",\
"or",\
"or_eq",\
"private",\
"protected",\
"public",\
"reflexpr",\
"register",\
"reinterpret_cast",\
"requires",\
"return",\
"short",\
"signed",\
"sizeof",\
"static",\
"static_assert",\
"static_cast",\
"struct",\
"switch",\
"synchronized",\
"template",\
"this",\
"thread_local",\
"throw",\
"true",\
"try",\
"typedef",\
"typeid",\
"typename",\
"union",\
"unsigned",\
"using",\
"virtual",\
"void",\
"volatile",\
"wchar_t",\
"while",\
"xor",\
"xor_eq"]
def highlight(line)
  if line.match(/".*"/) and not line.match(/#[a-z]* /)
    line
    string = line.scan(/".*"/)
    line.gsub!(/#{string.first}/, "<span class=\"str\">#{string.first}</span>")
  end
  if line.match(/#[a-z]* /)
    macro_line = line.split(/ +/)
    
    if macro_line.length > 1
      macroer, pred = macro_line[0], macro_line[1..].join(" ")
      
    else 
      macroer = macro_line[0]
      pred = ""
    end 
    
    macro_line.each_with_index {|token, index|
      if index == 0
        line.gsub!(/#{token}/, "<span class=\"macro\">#{token}</span>")
      else
        line.gsub!(/#{Regexp.escape(token)}/, "<span class=\"macro_pred\">#{pred}</span>")
      end
      }
    
  end

  if line.match(/\/\//)
      
    comment = line.scan(/.*(\/\/.*$)/)
      comment.each { |c|
        line.gsub!(/#{Regexp.escape(c.first)}/, "<span class=\"comment\">#{Regexp.escape(c.first)}</span>")
      }
  end
  # failed attempt at multiline comments
  # if line.match(/\/\*.*\*\//)
  #   comment = line.scan(/\/\*.*\*\//)
  #   
  #   comment.each { |c|
  #     line.gsub!(/#{c.first}/, "<span class=\"comment\">#{c.first}</span>")
  #   }
  # end
  if line.match(/[a-zA-z0-9]+[ +\t+][a-zA-z0-9]*\(\)/) # function declaration or definition
    f_name = line.scan(/[a-zA-z0-9]+[ +\t+]([a-zA-z0-9]*)\(\)/).first.first
  
    line.gsub!(/#{Regexp.escape(f_name)}/, "<span class=\"f_name\" id=\"#{f_name}\">#{Regexp.escape(f_name)}</span>")
  end

  if line.match(/[a-zA-Z_]*::/)
    namespace = line.scan(/([a-zA-Z]*)::/)
    namespace.each { |n|
      line.gsub!(/#{n.first}/, "<span class=\"namespc\">#{n.first}</span>")
    }
    
  end

  $cpp_keywords.each { |keyword|
    line.gsub!(/\b#{keyword}\b/, "<span class=\"keyword\">#{keyword}</span>")
  }
  return line
end
# makes the directory HTML if it doesn't exist
Dir.mkdir("HTML") unless File.exists?("HTML")

#the output buffer is where we will write our markup for XREF
output = open("HTML/#{ARGV[0]}.html", "w")
# writing the top part of the html, which includes .css, .js and jquery links. 
output.write("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"/><title> XREF </title><script src=\"https://code.jquery.com/jquery-1.11.3.min.js\"></script><script src=\"../search.js\"></script><link rel=\"stylesheet\" type=\"text/css\"href=\"../index.css\"/></head><body><form>Search: <input type=\"text\" id=\"search\"><a id=\"sub_btn\" href=\"\">Submit</a></form><table class=\"cross_ref\">")
# last_source and last files are for creating the interleaving background-colors in the source, and extending the background color of the source that corresponds to contiguous assembly lines to multiple lines. 
$last_source = ""
$last_file = ""
$row_color = "color1"
$source_color = "color5"
$line_printed = Hash.new # file -> lines -> bool

# writes the assembly and the source side by side in XREF page. 
def print_side_by_side(inst, file, line, output)
  if inst[2] == ["callq"] or inst[2] == ["jae"] or inst[2] == ["je"] or inst[2] == ["jne"] or inst[2] == ["jmp"] or inst[2] == ["jbe"] or inst[2] == ["jb"]
    inst = "<th class=\"inst0\">#{inst[0]}</th><th class=\"inst1\">#{inst[1]}</th><th class=\"inst2\">#{inst[2][0]}</th><th class=\"inst3\">#{inst[3][0]}</th><th class=\"inst31\"><a href=\"\##{inst[3][1][0]}\">#{inst[3][1][0]}</a></th>"

  else 
    inst = "<th class=\"inst0\">#{inst[0]}</th><th class=\"inst1\">#{inst[1]}</th><th class=\"inst2\">#{inst[2][0]}</th> <th class=\"inst3\">#{inst[3][0]}</th><th></th>"
  end

  source = "#{highlight(IO.readlines(file)[line].gsub("<", "&#60").gsub(">", "&#62"))}"
  # if the source line is the same as the previous one, don't print it
  if source == $last_source and file == $last_file
    print_source = ""
  else
    print_source = source
    if $source_color == "color5"
      $source_color = "color6"
    else
      $source_color = "color5"
    end
  end
  output.write("<tr class=#{$row_col}>#{inst}")
  if not $line_printed[file][line] or print_source == ""
    output.write("<th class=\"#{$source_color}\">#{print_source}</th></tr>")
    $line_printed[file][line] = true
  else
    output.write("<th class=\"color8\">#{print_source}</th></tr>")
  end
  $last_file = file
  $last_source = source
  if $row_col == "color1"
    $row_col = "color2"
  else $row_col = "color1"
  end
end

def print_file_name(file, output)
  output.write("<tr><th></th><th></th><th></th><th></th><th></th><th class=\"color3\">#{file}</th></tr>")
end

# assembly exists is a hashmap from filenames to list of bools. It stores whether in a given line in a given file has a corresponding assembly in dwarf dump or not. 
$assembly_exists = Hash.new # file -> lines -> bool
def print_previous_lines(file, line, output)

  i = 1
  lines_to_print = []
  until $assembly_exists[file][line - i] == true or line == i - 1 do
    $line_printed[file][line - i] = true
    lines_to_print.push(highlight(IO.readlines(file)[line - i].gsub("<", "&#60").gsub(">", "&#62")))
    i += 1
  end
  lines_to_print.reverse.each { |i|
    output.write("<tr><th></th><th></th><th></th><th></th><th></th><th class=\"color7\">#{i}</th></tr>")
  }

end


opened = Hash.new
last_file = ""
# iterate over all assembly functions and write in XREF, seperating each func block with a clear label that contains the name of the function block.
funcs.each { |f, body|
  func_written = false
  # iterate over each instruction in the assembly block
  body.each {|i|
    # if the dwarf dump has the address of the instruction then...
    all_dwarf.each_with_index { |dwarf, index|
      if dwarf.has_key?(i[0])
        if not func_written
          output.write("<tr class=\"empty_row\"><th></th><th></th><th></th><th></th><th></th></tr><tr><th class=\"color4\" id=\"#{f}\">#{f}</th><th></th><th></th><th></th><th></th><th></th></tr>")
        end
        func_written = true
        file = "#{all_dirs[index][Integer(all_file_names[index][Integer(dwarf[i[0]][2])][1])]}/#{all_file_names[index][Integer(dwarf[i[0]][2])][0]}"

        line = Integer(dwarf[i[0]][0])
        if not opened.has_key? file
          opened[file] = open(file, "r")
        end
        if file != last_file
          print_file_name(file, output)
        end
        if $assembly_exists[file] == nil
          $assembly_exists[file] = [false] * %x{wc -l '#{file}'}.to_i 
        end

        if $line_printed[file] == nil
          $line_printed[file] = [false] * %x{wc -l '#{file}'}.to_i 
        end
        $assembly_exists[file][line] = true

        # expensive function
        print_previous_lines(file, line, output)

        print_side_by_side(i, file, line, output)

      end
    }
    last_file = file
  }
}


# printing unprinted lines at the very end of the webpage
$prev_file = ""
$line_printed.each { |file, lines|
  lines.each_with_index { |bool, index|
    if not bool
      if not $prev_file == file
        print_file_name(file, output)
      end
      output.write("<tr><th></th><th></th><th></th><th></th><th></th><th class=\"color7\">#{highlight(IO.readlines(file)[index])}</th></tr>")
      $prev_file = file
    end
  }
}

# done writing output buffer
output.write("</table></body></html>")
output.close()

# create index.html with link to the main function of the file that we just created using the "output" buffer
index = open("HTML/index.html", "w")
index.write("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"/><title> XREF </title><script src=\"https://code.jquery.com/jquery-1.11.3.min.js\"></script><script src=\"./search.js\"></script><link rel=\"stylesheet\" type=\"text/css\"href=\"./index.css\"/></head><body><a href=\"#{ARGV[0]}.html?#main\"> Go to main </a><br/>XREF was run using the command \"ruby parse.rb #{ARGV[0]}\" in the directory \"#{Dir.pwd}\" at time \"#{Time.now}</body></html>\"")
