using IW5Project.Model;
using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels.Commands.Rating
{
    public class AddRatingCommand<T> : CommandBase<ViewModelCollection<T>>
        where T : BaseModel, new()
    {
        public AddRatingCommand(ViewModelCollection<T> viewModelCollection) : base(viewModelCollection)
        {
        }

        public override void Execute(object parameter)
        {
            this.ViewModel.Service.Add(ViewModel.NewItem);
            this.ViewModel.Items.Add(ViewModel.NewItem);
            this.ViewModel.NewItem = null;
        }
    }
}