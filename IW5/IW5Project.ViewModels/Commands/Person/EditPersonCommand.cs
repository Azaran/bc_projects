using System.Diagnostics;
using IW5Project.Model;
using IW5Project.ViewModels.Framework.Commands;
using IW5Project.ViewModels.Framework.ViewModels;

namespace IW5Project.ViewModels.Commands.Person
{
    class EditPersonCommand<T> : CommandBase<ViewModelCollection<T>>
        where T : Model.Persons.Person, new()
    {
        public EditPersonCommand(ViewModelCollection<T> viewModelCollection) : base(viewModelCollection)
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
                    this.ViewModel.Status = "Selected 'Person' was loaded. Now you can edit it.";
                }
            }
            else
            {
                Debug.WriteLine("null");
            }
        }
    }
}