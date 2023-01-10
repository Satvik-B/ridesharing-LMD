import folium
import random

used_colors = set([])
def randomColor():
    global used_colors
    i = 0
    color = "#"+''.join([random.choice('0123456789ABCDEF') for j in range(6)])
    while (color in used_colors) and (i<10):
        i = i+1
        color = "#"+''.join([random.choice('0123456789ABCDEF') for j in range(6)])

    if(i>=10):
        raise "Ran out of tries for colors"
    
    used_colors.add(color)
    return color
    
if __name__=="__main__":
    random.seed(10)

    pts = open("input.txt", "r")
    # depot location
    loc = pts.readline()
    depot_x_loc = float(loc.split(" ")[0])
    depot_y_loc = float(loc.split(" ")[1])

    clusters = [int(v) for v in open('output.txt').read().split()]


    pts.readline() # the dimensions of the bin
    num_ct = int(pts.readline()) # the number of packages
    colors = []
    for i in range(num_ct):
        colors.append(randomColor())

    results = []
    map = folium.Map(location=[42.150, -88.034])
    folium.Marker(location=[42.129, -88.027], popup =  'Depot').add_to(map)


    for i in range(num_ct):
        line = pts.readline()
        pt_x_loc = float(line.split(" ")[0])
        pt_y_loc = float(line.split(" ")[1])
        
        # results.append([pt_x_loc, pt_y_loc, colors[clusters[i]]])
        folium.Circle(
            radius = 50, 
            location=[pt_x_loc, pt_y_loc],
            fill = False,
            popup = f"lat={pt_x_loc}, lng={pt_y_loc}",
            color = colors[clusters[i]]
        ).add_to(map)

    map.save('clusters.html')
