using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;
using IW5Project.ViewModels.FileManager;

namespace IW5Project.ViewModels.Commands.Movie
{
    class InsertPoster : CommandBase<ViewModelCollection<Model.Movies.Movie>>
    {

        public InsertPoster(ViewModelCollection<Model.Movies.Movie> viewModelCollection) : base(viewModelCollection)
        {
        }

        public override void Execute(object parameter)
        {
            var fileManager = new FileManager.FileManager();
            FileManager.FileManager.StatusOfLoadingFile result = fileManager.GetFile(FileManager.FileManager.TypeOfFileLoading.Poster,this.ViewModel.NewItem.Id);
            if (result == FileManager.FileManager.StatusOfLoadingFile.Ok)
            {
                this.ViewModel.NewItem.SetPoster(fileManager.Extension);
                this.ViewModel.ImagePath =
                    fileManager.GethPath(IW5Project.ViewModels.FileManager.FileManager.TypeOfFileLoading.Poster,
                        this.ViewModel.NewItem.PosterEnding, this.ViewModel.NewItem.Id);
                this.ViewModel.PosterStatus = "Poster was loaded!";
            }
            else if (result == FileManager.FileManager.StatusOfLoadingFile.WrongFormat)
                this.ViewModel.PosterStatus = "Only supported formats are jpg, png.";
            else if (result == FileManager.FileManager.StatusOfLoadingFile.NoFileSelected)
                this.ViewModel.PosterStatus = "No file was selected.";
        }
    }

}

