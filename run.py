#!/usr/bin/python3
import csv
import sys

def startLogger(widget,outfile):
    return f"""
# Change directory to the Logger directory
cd ..; cd Logger

# Start the Logger for {widget}, writing to {outfile}
../bin/Logger Logger {outfile} tcp://127.0.0.1:1300 {outfile}.txt &""".format(widget=widget,outfile=outfile)

def startWidget(widget, output, cli):
    return f"""#!/bin/sh
#### Starting {widget} ####
# Change directory to the {widget} directory
cd {widget}

# Start the {widget} widget {"with the cli argument" if cli else ""}
../bin/{widget} {widget} {widget} tcp://127.0.0.1:1313 {cli if cli else output} tcp://127.0.0.1:1300 {cli} &""".format(widget=widget,cli=cli,output=output)

def emit(row, cli):
    if row["cli"]:
        if not cli or cli not in row["cli"]:
            return f"""### Usage: {row["widget"]} requires a CLI argument: {row["cli"]}"""
    outstring = startWidget(row["widget"], row["outfile"], cli)
    if cli:
        outstring += startLogger(row["widget"], cli)
    elif row["outfile"] != "":
        if row["outfile"][0] == "[":
            # in the form: [a|b|c], start a Logger for each outfile
            elements = row["outfile"][1:-1].split('|')
            for element in elements:
                outstring += startLogger(row["widget"], element)
        else:
            outstring += startLogger(row["widget"], row["outfile"])

    if row["infile"] != "":
        outstring +=f"""
  # Change directory to the Feeder directory
  cd ..; cd Feeder

  # Start the Feeder for {row["widget"]} with requested infile(s)
  ../bin/Feeder Feeder {row["widget"]} tcp://127.0.0.1:1313 """.format(row=row)
        if row["infile"][0] == "[":
            # in the form: [a|b|c], send all files to Feeder
            filelist = row["infile"][1:-1].split('|')
            for infile in filelist:
                outstring += infile + ".txt "
            outstring += " &"
        else:
            outstring +=f"""{row["infile"]}.txt &""".format(row=row)

    outstring += f"""
  cd ..
  #### Done with {row["widget"]} ####""".format(row=row)
    return(outstring)

widget="PixelFactory"
if len(sys.argv) > 1:
    widget=sys.argv[1]

cli=""
if len(sys.argv) > 2:
    cli=sys.argv[2]

with open('run.csv', 'r') as csvfile:
  reader = csv.DictReader(csvfile, delimiter=',')
  for row in reader:
      # print(row["widget"])
      if row["widget"] == widget:
          print(emit(row, cli))
          sys.exit(0)
