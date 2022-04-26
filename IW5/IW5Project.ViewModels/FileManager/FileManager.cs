using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IW5Project.ViewModels.FileManager
{
    public class FileManager
    {
        public enum TypeOfFileLoading { Avatar, Poster };

        public enum StatusOfLoadingFile { Ok, WrongFormat, NoFileSelected };
        public string Extension { get; set; }
        private string PathToAvatarsFolder;
        private string PathToPostersFolder;
        private string CurrentDirectory;
        public FileManager()
        {
            this.PathToAvatarsFolder = @"\Avatars";
            this.PathToPostersFolder = @"\Posters";
            this.CurrentDirectory = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location);
        }

        public StatusOfLoadingFile GetFile(TypeOfFileLoading type, Guid NewFileName)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            openFileDialog1.Filter = "Image Files (JPG,PNG)|*.JPG;*.PNG";
            DialogResult result = openFileDialog1.ShowDialog();
            if (result == DialogResult.OK)
            {
                this.Extension = System.IO.Path.GetExtension(openFileDialog1.FileName);
                if (this.Extension != ".jpg" && this.Extension != ".png")
                    return StatusOfLoadingFile.WrongFormat;


                string file = openFileDialog1.FileName;

                if (!System.IO.Directory.Exists(CurrentDirectory + PathToAvatarsFolder))
                {
                    System.IO.Directory.CreateDirectory(CurrentDirectory + PathToAvatarsFolder);
                }
                if (!System.IO.Directory.Exists(CurrentDirectory + PathToPostersFolder))
                {
                    System.IO.Directory.CreateDirectory(CurrentDirectory + PathToPostersFolder);
                }


                if (type == TypeOfFileLoading.Avatar)
                    System.IO.File.Copy(file, CurrentDirectory + PathToAvatarsFolder + "\\" + NewFileName + this.Extension, true);
                else if (type == TypeOfFileLoading.Poster)
                    System.IO.File.Copy(file, CurrentDirectory + PathToPostersFolder + "\\" + NewFileName + this.Extension, true);

            }
            else
            {
                return StatusOfLoadingFile.NoFileSelected;
            }
            return StatusOfLoadingFile.Ok;
        }

        public void DeleteFile(TypeOfFileLoading type, string OptExtension, Guid NameToDelete)
        {
            if (type == TypeOfFileLoading.Avatar)
            {
                if (System.IO.File.Exists(CurrentDirectory + PathToAvatarsFolder + "\\" + NameToDelete + OptExtension))
                {
                    System.IO.File.Delete(CurrentDirectory + PathToAvatarsFolder + "\\" + NameToDelete + OptExtension);
                }

            }
            else if (type == TypeOfFileLoading.Poster)
            {
                Console.WriteLine(CurrentDirectory + PathToPostersFolder + "\\" + NameToDelete + OptExtension);
                if (System.IO.File.Exists(CurrentDirectory + PathToPostersFolder + "\\" + NameToDelete + OptExtension))
                {
                    System.IO.File.Delete(CurrentDirectory + PathToPostersFolder + "\\" + NameToDelete + OptExtension);
                }
            }
        }

        public string GethPath(TypeOfFileLoading type, string OptExtension, Guid NameToDelete)
        {
            if (type == TypeOfFileLoading.Avatar)
            {
                if (System.IO.File.Exists(CurrentDirectory + PathToAvatarsFolder + "\\" + NameToDelete + OptExtension))
                {
                    return CurrentDirectory + PathToAvatarsFolder + "\\" + NameToDelete + OptExtension;
                }

            }
            else if (type == TypeOfFileLoading.Poster)
            {
                Console.WriteLine(CurrentDirectory + PathToPostersFolder + "\\" + NameToDelete + OptExtension);
                if (System.IO.File.Exists(CurrentDirectory + PathToPostersFolder + "\\" + NameToDelete + OptExtension))
                {
                    return CurrentDirectory + PathToPostersFolder + "\\" + NameToDelete + OptExtension;
                }
            }
            return "";
        }



    }
}
