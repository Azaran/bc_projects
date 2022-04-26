using System.Diagnostics;
using IW5Project.Model;
using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels.Commands
{
    public class EditCommand<T> : CommandBase<ViewModelCollection<T>>
        where T : BaseModel, new()
    {
        public EditCommand(ViewModelCollection<T> viewModelCollection) : base(viewModelCollection)
        {
        }

        public override void Execute(object parameter)
        {
            var typeItem = parameter as T;

            if (typeItem != null)
            {
                Debug.WriteLine(typeItem.Id);
                ViewModel.NewItem = typeItem;
                ViewModel.Items.Remove(typeItem);
                ViewModel.Service.Delete(typeItem);
                ViewModel.Service.Save();
            }
            else
            {
                Debug.WriteLine("null");
            }
        }
    }
}