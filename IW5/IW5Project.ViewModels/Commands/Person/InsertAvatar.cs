using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;
using IW5Project.ViewModels.FileManager;

namespace IW5Project.ViewModels.Commands.Person
{

    class InsertAvatar : CommandBase<ViewModelCollection<Model.Persons.Person>>
    {
        public InsertAvatar(ViewModelCollection<Model.Persons.Person> viewModelCollection) : base(viewModelCollection)
        {

        }
        public override void Execute(object parameter)
        {
            var fileManager = new FileManager.FileManager();
            FileManager.FileManager.StatusOfLoadingFile result = fileManager.GetFile(FileManager.FileManager.TypeOfFileLoading.Poster, this.ViewModel.NewItem.Id);
            if (result == FileManager.FileManager.StatusOfLoadingFile.Ok)
            {
                this.ViewModel.NewItem.SetAvatar(fileManager.Extension);
                this.ViewModel.AvatarStatus = "Avatar was loaded!";
            }
            else if (result == FileManager.FileManager.StatusOfLoadingFile.WrongFormat)
                this.ViewModel.PosterStatus = "Only supported formats are jpg, png.";
            else if (result == FileManager.FileManager.StatusOfLoadingFile.NoFileSelected)
                this.ViewModel.PosterStatus = "No file was selected.";
        }
    }
}
