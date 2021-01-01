class Frame(object):
    """
    Frame object represents and builds a single frame.
    Use with accompanying arduino code:
        for 128x64 i2c displays.
        prints 144 character string in format 18x8
    each line must be instantiated where needed.
    unistialised lines will be padded with ' '....
    you only need to initialise lines where data is needed to be displayed
    """

    def __init__(self):
        self.__lines = [None for x in range(8)]

    def line(self, lineNumber):
        """Returns initialised line object """
        if lineNumber > 8 or lineNumber < 1:
            raise ValueError('lineNumber should be between 1 and 8')
        elif not self.__lines[lineNumber-1]:
            raise Exception(f'line {lineNumber} is not initiated')
        return self.__lines[lineNumber-1]

    def init_line(self, lineNumber, columns=1):
        """initiates a line for formatting
        throws error if line has already been initiated
        throws error if columns are too high"""
        if self.__get_line_number(lineNumber):
            raise Exception(f'line {lineNumber} is already initiated')
        elif columns < 1 or columns > 3:
            raise Exception(f'line {lineNumber} can only have beetween 1 and 3 columns')
        else:
            self.__lines[lineNumber-1] = self.Line(columns)

    def __get_line_number(self, lineNumber):
        """returns line object or None for requested line"""
        if lineNumber > 8 or lineNumber < 1:
            raise ValueError('lineNumber should be between 1 and 8')
        return self.__lines[lineNumber-1]

    def getstring(self):
        """returns frame string"""
        frameString = ''
        for i in self.__lines:
            if i == None:
                frameString += ' '*18
            else:
                frameString += i.getstring()
        return frameString + '\n'

    def getstring_encoded(self):
        """returns encoded frame string bytes"""
        return self.getstring().encode('utf-8')


    class Line:
        """Line class to initialise line
        Takes integer of # of columns
        1 (18), 2 (9, 9), 3 (6, 6, 6) is available
        """

        def __init__(self, columns=1):
            """initiates new line with set amount of columns
            """
            self.columncount = columns
            self.columns = [' '*int(18/columns) for i in range(columns)]

        def setcolumntext(self, column, text, align='left'):
            """sets text to desired column.
            @param column : the desired column number
            @param text : the text to be displayed
            @param align : alignment of text
            """
            if column > self.columncount or column < 1:
                raise Exception(f'column must be between 1 and {self.columncount}')
            
            self.columns[column-1] = self.__aligntext(column, text, align)

        def __check_textlength(self, text):
            """checks text input length:
                prints textual warning if the text length is longer then column
                returns trimmed text if length is longer then column
            """

            if len(text) > (18 / self.columncount):
                print(f"WARN: text value '{text}' is too long for {self.columncount} and should be {self.__calc_maxlength()}")
                text = text[:int(18 / self.columncount)]
            return text

        def __aligntext(self, column, text, align):
            """Aligns the text on the column, depending on text size and available column size
            returns fully aligned text for the specified column
            """
            text = self.__check_textlength(text)
            if 'left' == align or 'right' == align:
                pad = self.__makepad(self.__calc_maxlength()-len(text))
                if 'left' == align:
                    return text + pad
                elif 'right' == align:
                    return pad + text
            elif 'center' == align:
                pad = self.__calc_maxlength() - len(text)
                remainder = pad % 2
                
                if 0 == pad:
                    return text
                
                if remainder:
                    pad = int((pad - remainder) / 2)
                else:
                    pad = int(pad / 2)

                if column % 2:
                    return self.__makepad(pad) + text + self.__makepad(pad + remainder)
                else:
                    return self.__makepad(pad + remainder) + text + self.__makepad(pad)
            else:
                raise ValueError("align must be either 'left', 'center', 'right'")

        def __makepad(self, number):
            """returns string pad for given length"""
            return ' ' * int(number)

        def __calc_maxlength(self):
            """calculates max length of each column in current line"""
            return int(18 / self.columncount)

        def getstring(self):
            """returns the entire line text"""
            return ''.join(self.columns)