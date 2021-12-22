#!/usr/bin/env python3
import sys
import json
import toml
import math

class point:
    def __init__(self,x,y):
        self.x_=x
        self.y_=y
    def __add__(self,other):
        return point(self.x_+other.x(),self.y_+other.y())
    def __sub__(self,other):
        return point(self.x_-other.x(),self.y_-other.y())
    def __mul__(self,factor):
        return point(self.x_*factor,self.y_*factor)
    def __rmul__(self,factor):
        return self*factor
    def length(self):
        return math.sqrt(self.x_**2+self.y_**2)
    def x(self):
        return self.x_
    def y(self):
        return self.y_

def print_help():
    print("create_template.py [options..]")
    print("    --title value  :necessary")
    print("    --destination value :optional must be end with '/'")
    print("    -h/--help      :show this help and exit")

def parse_args(argc,argv):
    if argc==1:
        print("error: no arguments given")
        exit(-1)
    real_args=argv[1:]
    itr_real_arg=iter(real_args)
    result={}
    while True:
        try:
            current_arg=next(itr_real_arg)
            if current_arg=="--title":
                result["title"]=next(itr_real_arg)
            elif current_arg=="--destination":
                result["destination"]=next(itr_real_arg)
            elif current_arg=="-h" or current_arg=="--help":
                print_help()
                exit(0)
        except StopIteration:
            if not "destination" in result:
                result["destination"]="./"
            break
    return result
        
def create_line(line_template,general_infos):
    diff=2*(general_infos["charge_radius"]+general_infos["start_offset"])+general_infos["gap"]
    pos_start=point(line_template["start"]["x"],line_template["start"]["y"])
    pos_end=point(line_template["end"]["x"],line_template["end"]["y"])
    vec_start_to_pos=pos_end-pos_start
    length_charge=(vec_start_to_pos).length()
    num_charges=int(length_charge//diff)+1
    charge_quantity=line_template["quantity"]
    result=[]
    for i in range(num_charges):
        current_pos=pos_start+(i/(num_charges-1))*vec_start_to_pos
        result.append({
            "charge quantity":charge_quantity,
            "position":{
                "x":current_pos.x(),
                "y":current_pos.y()
                }
            })
    return result

def create_circle(circle_template,general_infos):
    init_radius=general_infos["charge_radius"]+general_infos["start_offset"]
    angle=2*math.asin((2*init_radius+general_infos["gap"])/(2*circle_template["radius"]))
    num_charges=math.floor((2*math.pi)/angle)
    pos_centre=point(circle_template["centre"]["x"],circle_template["centre"]["y"])
    charge_quantity=circle_template["quantity"]
    result=[]
    for i in range(num_charges):
        current_angle=2*math.pi*(i/num_charges)
        current_pos=pos_centre+circle_template["radius"]*point(math.cos(current_angle),math.sin(current_angle))
        result.append({
            "charge quantity":charge_quantity,
            "position":{
                "x":current_pos.x(),
                "y":current_pos.y()
                }
            })
    return result

def main(argc,argv):
    input_infos=parse_args(argc,argv)
    template_template=toml.load(f"./{input_infos['title']}_template.toml")
    general_infos=template_template["general"]
    result={
            "inverse permittivity":general_infos["inverse_permittivity"],
            "fixed_charges":[]
            }
    if "lines" in template_template["patterns"]:
        for line in template_template["patterns"]["lines"]:
            result["fixed_charges"]+=create_line(line,general_infos)
    if "circles" in template_template["patterns"]:
        for circle in template_template["patterns"]["circles"]:
            result["fixed_charges"]+=create_circle(circle,general_infos)
    outfile=open(f"{input_infos['destination']}{input_infos['title']}.json","w")
    json.dump(result,outfile,sort_keys=True,indent=4)
    outfile.close()

if __name__ == "__main__":
    argv=sys.argv
    argc=len(argv)
    main(argc,argv)
