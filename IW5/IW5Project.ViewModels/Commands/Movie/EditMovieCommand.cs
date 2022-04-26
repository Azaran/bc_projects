using System.Diagnostics;
using System.Windows.Input;
using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels.Commands.Movie
{
    class EditMovieCommand<T> : CommandBase<ViewModelCollection<T>>
        where T : Model.Movies.Movie, new()
    {
        public EditMovieCommand(ViewModelCollection<T> viewModelCollection) : base(viewModelCollection)
        {
        }

        public override void Execute(object parameter)
        {
            var typeItem = parameter as T;

            if (typeItem != null)
            {
                Debug.WriteLine(typeItem.Id);
                ViewModel.NewItem = typeItem;
                if (!string.IsNullOrWhiteSpace(this.ViewModel.NewItem.Name))
                {
                    ViewModel.Items.Remove(typeItem);
                    ViewModel.Service.Delete(typeItem);
                    ViewModel.Service.Save();
                    this.ViewModel.Status = "Selected 'Movie' was loaded. Now you can edit it.";
                }
            }
            else
            {
                Debug.WriteLine("null");
            }
        }
    }
}