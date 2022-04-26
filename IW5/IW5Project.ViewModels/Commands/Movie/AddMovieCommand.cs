using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IW5Project.ViewModels.Commands.Movie
{
    class AddMovieCommand : CommandBase<ViewModelCollection<Model.Movies.Movie>>
    {
        public AddMovieCommand(ViewModelCollection<Model.Movies.Movie> viewModelCollection) : base(viewModelCollection)
        {
        }

        public override void Execute(object parameter)
        {
            if (this.ViewModel.NewItem == null)
            {
                this.ViewModel.NewItem = new Model.Movies.Movie();
                this.ViewModel.Status = "Object 'Movie' was created.";
            }
            else {
                if (!string.IsNullOrWhiteSpace(this.ViewModel.NewItem.Name))
                {
                    this.ViewModel.Service.Add(this.ViewModel.NewItem);
                    this.ViewModel.Items.Add(this.ViewModel.NewItem);
                    this.ViewModel.Service.Save();
                    this.ViewModel.Status = "Object 'Person' was saved to the database.";
                }
                this.ViewModel.NewItem = null;
            }

        }
    }
}
