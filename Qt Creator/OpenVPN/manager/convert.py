from base64 import b64decode,b64encode
from os.path import isdir,isfile
from os import makedirs


def file_to_base(path):
    try:
        with open(path,"rb") as file:
            base = b64encode(file.read()).decode("utf-8")
        return base
    except:
        return None


def base_to_file(base, path, file):
    if not isdir(path):
        makedirs(path)
    if isfile("{0}/{1}".format(path, file)) == False:
        try:
            with open("{0}/{1}".format(path, file), "wb") as ffile:
                ffile.write(b64decode(base))
        except:
            print("Не удалось создать файл {0}/{1}".format(path, file))
            return None
    return "{0}/{1}".format(path, file)

