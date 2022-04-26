using IW5Project.Model;
using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels.Commands
{
    public class RemoveCommand<T> : CommandBase<ViewModelCollection<T>>
        where T: BaseModel, new()
    {
        public RemoveCommand(ViewModelCollection<T> viewModelCollection) : base(viewModelCollection)
        {
        }

        public override void Execute(object parameter)
        {
            var typeItem = parameter as T;
            if (typeItem != null)
            {
                ViewModel.Items.Remove(typeItem);
            }
        }
    }
}