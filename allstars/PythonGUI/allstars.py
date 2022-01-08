# -*- coding: utf-8 -*-
import wx
import zipfile
import csv
import dbfread
import os
import sys
import io
import tempfile
import datetime


class Player:
    def __init__(self, x):
        x = x.rstrip()
        
        self.last_name = x[0:25]
        self.first_name = x[25:50]
        self.school_code = x[50:54].strip()
        self.grade = x[54:55]
        self.id = x[57:61]
        self.nwsrs_rating = int(x[62:66].strip())
        self.num_games = int(x[68:72].strip())
        self.games_ytd = int(x[73:77].strip())
        self.highest_rating_this_year = int(x[78:82].strip())
        self.lowest_rating = int(x[84:88].strip())
        self.start_rating = int(x[90:94].strip())
        self.kirks_code = x[99:100]
        self.last_action_date = x[101:109]
        self.rating_basis = x[110:111]
        self.uscf_last_name = x[112:137]
        self.uscf_first_name = x[137:162]
        self.uscf_id = x[162:170].strip()
        self.uscf_exp_date = x[171:179]
        self.uscf_rating = int(x[180:184].strip())
        self.uscf_prov_codes = x[186:187]
        self.uscf_rating_date = x[191:199]
        self.state = x[200:202]
        
        self.calc_highest_rating = 0

    def isAdult(self):
        return self.grade == "N"
    
    def isUSCFProvOrExpired(self):
        return self.uscf_prov_codes == "E" or self.uscf_prov_codes == "P"

    def isUSCFProv(self):
        return self.uscf_prov_codes == "P"

        
class AllstarApp(wx.Frame):

    def __init__(self, parent, title):
        super(AllstarApp, self).__init__(parent, title=title,
            size=(350, 300))

        self.InitUI()
        
    def OnBrowseClicked(self, event): 
        btn = event.GetEventObject()
        
        if hasattr(btn, "associated_style"):
            style = btn.associated_style
        else:
            style = wx.FD_OPEN | wx.FD_FILE_MUST_EXIST
            
        with wx.FileDialog(self, "Select File", style=style) as fileDialog:
            if fileDialog.ShowModal() == wx.ID_CANCEL:
                return     # the user changed their mind

            pathname = fileDialog.GetPath()
            btn.associated_text.SetValue(pathname)
        
    def InitUI(self):
        panel = wx.Panel(self)

        #panel.SetBackgroundColour('#4f5049')
        #vbox = wx.BoxSizer(wx.VERTICAL)

        #midPan = wx.Panel(panel)
        ##midPan.SetBackgroundColour('#ededed')

        #vbox.Add(midPan, wx.ID_ANY, wx.EXPAND | wx.ALL, 20)
        
        font = wx.SystemSettings.GetFont(wx.SYS_SYSTEM_FONT)

        font.SetPointSize(9)

        vbox = wx.BoxSizer(wx.VERTICAL)

        todays_date = datetime.date.today()
        this_year = todays_date.year
        period_hbox = wx.BoxSizer(wx.HORIZONTAL)
        st1 = wx.StaticText(panel, label='Period')
        st1.SetFont(font)
        period_hbox.Add(st1, flag=wx.RIGHT, border=8)
        choices = [str(year) for year in range(this_year, 1990, -1)]
        self.year_combo = wx.ComboBox(panel, choices=choices)
        period_hbox.Add(self.year_combo, flag=wx.RIGHT, border=8)
        self.season_combo = wx.ComboBox(panel, choices=["Winter", "Spring", "Summer"])
        period_hbox.Add(self.season_combo, flag=wx.RIGHT, border=8)
        #tc = wx.TextCtrl(panel)
        #hbox1.Add(tc, proportion=1)
        vbox.Add(period_hbox, flag=wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, border=10)
        #vbox.Add((-1, 10))

        TDData_hbox = wx.BoxSizer(wx.HORIZONTAL)
        st2 = wx.StaticText(panel, label='TDData.zip file')
        st2.SetFont(font)
        TDData_hbox.Add(st2)
        self.tddata_file_tc = wx.TextCtrl(panel)
        TDData_hbox.Add(self.tddata_file_tc, proportion=1)
        self.tddata_browse = wx.Button(panel, label='Browse')
        self.tddata_browse.associated_text = self.tddata_file_tc
        TDData_hbox.Add(self.tddata_browse)
        vbox.Add(TDData_hbox, flag=wx.LEFT | wx.TOP, border=10)
        self.tddata_browse.Bind(wx.EVT_BUTTON, self.OnBrowseClicked) 

        #vbox.Add((-1, 10))

        nwsrs_hbox = wx.BoxSizer(wx.HORIZONTAL)
        st2 = wx.StaticText(panel, label='NWSRS Cumulative Report')
        st2.SetFont(font)
        nwsrs_hbox.Add(st2)
        self.nwsrs_file_tc = wx.TextCtrl(panel)
        nwsrs_hbox.Add(self.nwsrs_file_tc, proportion=1)
        self.nwsrs_browse = wx.Button(panel, label='Browse')
        self.nwsrs_browse.associated_text = self.nwsrs_file_tc
        nwsrs_hbox.Add(self.nwsrs_browse)
        vbox.Add(nwsrs_hbox, flag=wx.LEFT | wx.TOP, border=10)
        self.nwsrs_browse.Bind(wx.EVT_BUTTON, self.OnBrowseClicked) 

        #vbox.Add((-1, 10))

        self.uscf_file_text = []
        self.uscf_browse_buttons = []
        
        for i in range(4):
            uscf_hbox = wx.BoxSizer(wx.HORIZONTAL)
            st2 = wx.StaticText(panel, label='USCF Regular Supplement #{}'.format(i))
            st2.SetFont(font)
            uscf_hbox.Add(st2)
            uscf_file_tc = wx.TextCtrl(panel)
            uscf_hbox.Add(uscf_file_tc, proportion=1)
            self.uscf_file_text.append(uscf_file_tc)
            uscf_browse = wx.Button(panel, label='Browse')
            uscf_hbox.Add(uscf_browse)
            self.uscf_browse_buttons.append(uscf_browse)
            uscf_browse.associated_text = uscf_file_tc
            vbox.Add(uscf_hbox, flag=wx.LEFT | wx.TOP, border=10)
            uscf_browse.Bind(wx.EVT_BUTTON, self.OnBrowseClicked) 
    
            #vbox.Add((-1, 10))

        homeschool_hbox = wx.BoxSizer(wx.HORIZONTAL)
        st2 = wx.StaticText(panel, label='Homeschooler cities file')
        st2.SetFont(font)
        homeschool_hbox.Add(st2)
        self.homeschool_file_tc = wx.TextCtrl(panel)
        homeschool_hbox.Add(self.homeschool_file_tc, proportion=1)
        self.homeschool_browse = wx.Button(panel, label='Browse')
        homeschool_hbox.Add(self.homeschool_browse)
        self.homeschool_browse.associated_text = self.homeschool_file_tc
        vbox.Add(homeschool_hbox, flag=wx.LEFT | wx.TOP, border=10)
        self.homeschool_browse.Bind(wx.EVT_BUTTON, self.OnBrowseClicked) 

        #vbox.Add((-1, 10))

        output_hbox = wx.BoxSizer(wx.HORIZONTAL)
        st2 = wx.StaticText(panel, label='Output file')
        st2.SetFont(font)
        output_hbox.Add(st2)
        self.output_file_tc = wx.TextCtrl(panel)
        output_hbox.Add(self.output_file_tc, proportion=1)
        self.output_browse = wx.Button(panel, label='Browse')
        output_hbox.Add(self.output_browse)
        self.output_browse.associated_text = self.output_file_tc
        self.output_browse.associated_style = wx.FD_SAVE
        vbox.Add(output_hbox, flag=wx.LEFT | wx.TOP, border=10)
        self.output_browse.Bind(wx.EVT_BUTTON, self.OnBrowseClicked) 

        #vbox.Add((-1, 10))
        
        btn1 = wx.Button(panel, label='Start Allstar Calculation')
        vbox.Add(btn1)
        btn1.Bind(wx.EVT_BUTTON, self.DoCalculation)
        
        """
        hbox3 = wx.BoxSizer(wx.HORIZONTAL)
        tc2 = wx.TextCtrl(panel, style=wx.TE_MULTILINE)
        hbox3.Add(tc2, proportion=1, flag=wx.EXPAND)
        vbox.Add(hbox3, proportion=1, flag=wx.LEFT|wx.RIGHT|wx.EXPAND,
            border=10)
        
        vbox.Add((-1, 25))
        
        hbox4 = wx.BoxSizer(wx.HORIZONTAL)
        cb1 = wx.CheckBox(panel, label='Case Sensitive')
        cb1.SetFont(font)
        hbox4.Add(cb1)
        cb2 = wx.CheckBox(panel, label='Nested Classes')
        cb2.SetFont(font)
        hbox4.Add(cb2, flag=wx.LEFT, border=10)
        cb3 = wx.CheckBox(panel, label='Non-Project classes')
        cb3.SetFont(font)
        hbox4.Add(cb3, flag=wx.LEFT, border=10)
        vbox.Add(hbox4, flag=wx.LEFT, border=10)
        
        vbox.Add((-1, 25))
        
        hbox5 = wx.BoxSizer(wx.HORIZONTAL)
        btn1 = wx.Button(panel, label='Ok', size=(70, 30))
        hbox5.Add(btn1)
        btn2 = wx.Button(panel, label='Close', size=(70, 30))
        hbox5.Add(btn2, flag=wx.LEFT|wx.BOTTOM, border=5)
        vbox.Add(hbox5, flag=wx.ALIGN_RIGHT|wx.RIGHT, border=10)
        """
        
        panel.SetSizerAndFit(vbox)
        self.Fit()
        
        self.year_combo.SetValue("2021")
        self.season_combo.SetValue("Summer")
        self.tddata_file_tc.SetValue("C:\\Users\\drtod\\OneDrive\\Music\\Documents\\chess\\oscf\\allstars\\9-1-2020\\TDdata.zip")
        self.nwsrs_file_tc.SetValue("C:\\Users\\drtod\\OneDrive\\Music\\Documents\\chess\\oscf\\allstars\\9-1-2020\\cumulative_events_OR.txt")
        self.homeschool_file_tc.SetValue("C:\\Users\\drtod\\OneDrive\\Music\\Documents\\chess\\oscf\\allstars\\9-1-2020\\homeschool.txt")
        self.uscf_file_text[0].SetValue("C:\\Users\\drtod\\OneDrive\\Music\\Documents\\chess\\oscf\\allstars\\9-1-2020\\RSQ2005.zip")
        self.uscf_file_text[1].SetValue("C:\\Users\\drtod\\OneDrive\\Music\\Documents\\chess\\oscf\\allstars\\9-1-2020\\RSQ2006.zip")
        self.uscf_file_text[2].SetValue("C:\\Users\\drtod\\OneDrive\\Music\\Documents\\chess\\oscf\\allstars\\9-1-2020\\RSQ2007.zip")
        self.uscf_file_text[3].SetValue("C:\\Users\\drtod\\OneDrive\\Music\\Documents\\chess\\oscf\\allstars\\9-1-2020\\RSQ2008.zip")
        self.output_file_tc.SetValue("C:\\Users\\drtod\\OneDrive\\Music\\Documents\\chess\\oscf\\allstars\\9-1-2020\\new_gui_output.txt")

    def process_tourney_results(self, cumul, or_players, nwsrs_map):
        ESTABLISHED_RATING = 15
        for fullid, start_rating, end_rating, total_games, games_this_event in cumul:
            games_at_start = total_games - games_this_event

            assert(len(fullid) == 8)
            id = fullid[4:]

            if id not in nwsrs_map:
                continue
            
            player_index = nwsrs_map[id]
        
            this_player = or_players[player_index]

            if games_at_start >= ESTABLISHED_RATING:
                if start_rating > this_player.calc_highest_rating:
                    this_player.calc_highest_rating = start_rating
            if total_games >= ESTABLISHED_RATING:
                if end_rating > this_player.calc_highest_rating:
                    this_player.calc_highest_rating = end_rating

    def process_uscf_supp(self, raw_uscf_data, or_players, uscf_map, start_date):
        for raw_id, raw_rating in raw_uscf_data.items():
            if raw_id not in uscf_map:
                continue
            player_index = uscf_map[raw_id]

            this_player = or_players[player_index]

            if this_player.isAdult() or this_player.isUSCFProv():
                continue;
                
            try:
                temp_date = datetime.datetime.strptime(this_player.uscf_exp_date, "%m/%d/%y").date()
            except Exception:
                print("Bad USCF date:", this_player.uscf_exp_date)
                continue
            if temp_date < start_date:
                continue

            if raw_rating > this_player.calc_highest_rating:
                this_player.calc_highest_rating = raw_rating
                if this_player.nwsrs_rating > this_player.calc_highest_rating:
                    this_player.calc_highest_rating = this_player.nwsrs_rating
            
    def DoCalculation(self, event):
        year_selected = self.year_combo.GetValue()
        if year_selected == "":
            wx.MessageBox("No year selected", 'Error', wx.OK | wx.ICON_WARNING)
            return
        
        season_selected = self.season_combo.GetValue()
        if season_selected == "":
            wx.MessageBox("No year selected", 'Error', wx.OK | wx.ICON_WARNING)
            return
        
        # Verify TDData zip present.
        tddata_filename = self.tddata_file_tc.GetValue()
        if tddata_filename == "":
            wx.MessageBox("TDData filename not specified", 'Error', wx.OK | wx.ICON_WARNING)
            return
        
        # Verify homeschool file present.
        home_filename = self.homeschool_file_tc.GetValue()
        if home_filename == "":
            wx.MessageBox("Homeschool database filename not specified", 'Error', wx.OK | wx.ICON_WARNING)
            return

        # Verify that cumulative tourney report is present.
        nwsrs_filename = self.nwsrs_file_tc.GetValue()
        if nwsrs_filename == "":
            wx.MessageBox("NWSRS cumulative tourney report filename not specified", 'Error', wx.OK | wx.ICON_WARNING)
            return
        
        # Verify that output file is specified.
        output_filename = self.output_file_tc.GetValue()
        if output_filename == "":
            wx.MessageBox("Output filename not specified", 'Error', wx.OK | wx.ICON_WARNING)
            return
        
        # End of dialog verification.
        
        year_selected = int(year_selected)
        if season_selected == "Spring":
            start_date = datetime.date(year_selected, 1, 1)
            end_date = datetime.date(year_selected, 4, 30)
        elif season_selected == "Summer":
            start_date = datetime.date(year_selected, 5, 1)
            end_date = datetime.date(year_selected, 8, 31)
        elif season_selected == "Winter":
            start_date = datetime.date(year_selected, 9, 1)
            end_date = datetime.date(year_selected, 12, 31)
        else:
            wx.MessageBox("Invalid season selected", 'Error', wx.OK | wx.ICON_WARNING)
            return
        
        # --------------------------------------------------------
        # Get allcodes.csv and rate*.dat file from TDData.ziop.
        input_zip = zipfile.ZipFile(tddata_filename)
        namelist = input_zip.namelist()
        #print("namelist", namelist)
        if "allcodes.csv" not in namelist:
            wx.MessageBox("allcodes.csv not present in TDData file", 'Error', wx.OK | wx.ICON_WARNING)
            return
        allcodes_csv = input_zip.read("allcodes.csv")
        ratelist = list(filter(lambda x: x.startswith("rate"), namelist))
        if len(ratelist) != 1:
            wx.MessageBox("Not exactly 1 file starting with rate in TDData file", 'Error', wx.OK | wx.ICON_WARNING)
            return
        schoolcodes = {}
        for code in allcodes_csv.decode('ANSI').split('\n'):
            fields = code.split(',')
            if len(fields) >= 3:
                schoolcodes[fields[0]] = fields[3]
        
        rate_dat = input_zip.read(ratelist[0])
        or_players = []
        player_index = 0
        nwsrs_map = {}
        uscf_map = {}
        #print("rate_dat:", type(rate_dat), len(rate_dat))
        for rate_line in rate_dat.decode('ANSI').split('\n'):
            if len(rate_line) < 200: break
            #print(player_index, rate_line)
            p = Player(rate_line)
            if p.state == "OR":
               or_players.append(p)
               nwsrs_map[p.id] = player_index
               if p.uscf_id != "":
                   uscf_map[p.uscf_id] = player_index
               #logfile << p.last_name << " " << p.first_name << " " << p.id << " " << p.uscf_id << " " << player_index << std::endl;
               player_index += 1

        # --------------------------------------------------------
        # Load homeschool file.
        with open(home_filename) as f:
            home_lines = f.read().splitlines() 
        #print("home_lines:", home_lines)
        home_dict = dict(zip(home_lines[0::2], home_lines[1::2]))
        #print("home_dict:", home_dict)
 
        # --------------------------------------------------------
        # Load and process cumulative tourney report file.
        with open(nwsrs_filename) as f:
            nwsrs_lines = f.read().splitlines()
        cumul = []
        in_region = False
        for nwsrs_line in nwsrs_lines:
            if "pos last name first" in nwsrs_line:
                in_region = True
            elif "The total Number" in nwsrs_line:
                in_region = False
            elif in_region:
                cutline = nwsrs_line[27:].replace("/"," ")
                csplit = cutline.split()
                num_games = 0
                for res in csplit[4:]:
                    if ("W" in res or "L" in res or "D" in res) and res != "WF" and res != "LF":
                        num_games += 1
                cumul.append((csplit[0], int(csplit[1].strip()), int(csplit[2].strip()), int(csplit[3].strip()), num_games))
        
        # --------------------------------------------------------
        # Verify and unzip USCF supplements.
        uscf_extracts = []
        for uscf_tc in self.uscf_file_text:
            uscf_filename = uscf_tc.GetValue()
            if uscf_filename == "":
                wx.MessageBox("USCF Supplement filename not specified", 'Error', wx.OK | wx.ICON_WARNING)
                return
            input_zip = zipfile.ZipFile(uscf_filename)
            namelist = input_zip.namelist()
            #print("namelist", uscf_filename, namelist)
            
            tarlist = list(filter(lambda x: "TARSFLE1" in x, namelist))
            if len(tarlist) != 1:
                print("Not exactly 1 file contain TARSFLE1 in USCF supplement zip file")
                return
            uscf_extracts.append(input_zip.read(tarlist[0]))
        
        raw_uscf_data = {}
        for uscf_extract in uscf_extracts:
            handle, temp_sig = tempfile.mkstemp()
            with os.fdopen(handle, "wb") as f:
                f.write(uscf_extract)

            in_db = dbfread.DBF(temp_sig)
            #in_db = dbfread.DBF(memory_file)
#            names = []
#            for field in in_db.fields:
#            for field in in_db.header.fields:
#                names.append(field.name)
#            raw_uscf_data.append(names)
            for rec in in_db:
                values = list(rec.values())
                if values[3] != "OR":
                    continue
                skip = False
#                print("values:", values)
                for j in range(len(values)):
                    if isinstance(values[j], str):
                        if "INACTIVE ID" in values[j]:
                            skip = True
                            break
                        values[j] = values[j].replace("*","")
                    rating = values[4]
                if skip:
                    continue
                if "/" not in rating: # skip provisional ratings
                    rating = rating.strip()
                    if rating.isnumeric():
                        if values[1] in raw_uscf_data:
                            if int(rating) > raw_uscf_data[values[1]]:
                                raw_uscf_data[values[1]] = int(rating)
                        else:
                            raw_uscf_data[values[1]] = int(rating)
#                raw_uscf_data.append(rec.fieldData)
#            in_db.close()    
            os.unlink(temp_sig)            
            
#        for raw_data in raw_uscf_data.items():
#            print(raw_data)
            
        self.process_tourney_results(cumul, or_players, nwsrs_map)
        self.process_uscf_supp(raw_uscf_data, or_players, uscf_map, start_date)
        
        or_by_grade = [[] for _ in range(14)]
        
        # Get player indices by grade.
        for i in range(len(or_players)):
            grade_index = ord(or_players[i].grade[0]) - ord('A')
            if grade_index < 0 or grade_index > 13:
                print("Bad grade for player:", or_players[i].last_name, or_players[i].first_name)
                sys.exit(-1)
                
            if or_players[i].calc_highest_rating > 0:
                or_by_grade[grade_index].append(i)

        grade_names = ["Kindergarten",
                        "1st Grade",
                        "2nd Grade",
                        "3rd Grade",
                        "4th Grade",
                        "5th Grade",
                        "6th Grade",
                        "7th Grade",
                        "8th Grade",
                        "Freshmen",
                        "Sophmores",
                        "Juniors",
                        "Seniors"]

        def isop(i):
            return or_players[i].calc_highest_rating

        with open(output_filename, 'w') as f:
            for i in range(12, -1, -1):
                or_by_grade[i].sort(reverse=True, key=isop)
                print(grade_names[i], file=f)
                #csvfile << grade_names[i] << ",,," << std::endl;
                num_players_on_list = 10
    
                if len(or_by_grade[i]) < 10:
                    num_players_on_list = len(or_by_grade[i])
    
                # By default there are 10 players on the list except for when there is a tie for 10th place.
                if len(or_by_grade[i]) > 10:
                    # Get the 10th player
                    star10 = or_players[or_by_grade[i][9]]
                    # For 11th position and onwards.
                    for j in range(10, len(or_by_grade[i]) + 1):
                        this_star = or_players[or_by_grade[i][j]]
                        # If there is a tie with the 10th player then add an additional player to the list.
                        if this_star.calc_highest_rating == star10.calc_highest_rating:
                            num_players_on_list += 1
                        else:
                            break
    
                in_tie = False
                start_tie = 0
    
                for j in range(num_players_on_list):
                    star = or_players[or_by_grade[i][j]]
                    full_name = star.last_name + ", " + star.first_name
                    if star.school_code not in schoolcodes:
                        wx.MessageBox(f"Student school code {star.school_code} not in schoolcode file.", 'Error', wx.OK | wx.ICON_WARNING)
                        wx.Exit()
                        
                    city = schoolcodes[star.school_code]
                    if city == "":
                        if full_name in home_dict:
                            city = home_dict[full_name]
         
                    position_str = ""
    
                    # If there is another player on the list after us.
                    if j != num_players_on_list - 1:
                        next_star = or_players[or_by_grade[i][j+1]]
                        if star.calc_highest_rating == next_star.calc_highest_rating:
                            if in_tie:
                                position_str += "T" + str(start_tie)
                            else:
                                in_tie = True
                                start_tie = j+1
                                position_str += "T" + str(start_tie)
                        else:
                            if in_tie:
                                in_tie = False;
                                position_str += "T" + str(start_tie)
                            else:
                                position_str += str(j+1)
                    else:
                        if in_tie:
                            position_str += "T" + str(start_tie)
                        else:
                            position_str += str(j+1)
                        
                    print(f"{position_str:>4} {full_name:<30} {city:<20} {star.calc_highest_rating:>4}", file=f)
                    #csvfile << position_str.str() << "," << "\"" << full_name << "\"," << city << "," << star.calc_highest_rating << std::endl;
                    #std::cout << std::endl;
    
                print("\n", file=f)
                #if(i != 0) {
                #    csvfile << ",,," << std::endl << ",,," << std::endl;
                #}
        wx.MessageBox("Done!", 'Complete', wx.OK)
         
def main():
    app = wx.App()
    allstar_app = AllstarApp(None, title='OSCF Allstars Calculation')
    allstar_app.Show()
    app.MainLoop()

if __name__ == '__main__':
    main()