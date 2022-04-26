using IW5Project.Model;
using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels.Commands
{
    public class AddCommand<T> : CommandBase<ViewModelCollection<T>>
        where T : BaseModel, new()
    {
        public AddCommand(ViewModelCollection<T> viewModelCollection) : base(viewModelCollection)
        {
        }

        public override void Execute(object parameter)
        {
            ViewModel.Service.Add(ViewModel.NewItem);
            ViewModel.Items.Add(ViewModel.NewItem);

            ViewModel.NewItem = new T();
        }
    }
}