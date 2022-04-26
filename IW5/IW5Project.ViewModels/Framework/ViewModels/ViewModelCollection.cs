using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;
using IW5Project.Model;
using IW5Project.ViewModels.Commands;

/// <summary>
/// Inspired by TaskManager framework from hw labs
/// </summary>
namespace IW5Project.ViewModels.Framework.ViewModels
{
    public abstract class ViewModelCollection<T>: ViewModelBase<T>
        where T: BaseModel, new()
    {
        private string _status;
        private T _newItem;
        private string _imagePath;
        public ObservableCollection<T> Items
        {
            get
            {
                return this.Service.GetObservableCollection();
            }
        }

        public T NewItem
        {
            get
            {
                return this._newItem;
            }
            set
            {
                if (!Equals(_newItem, value))
                {
                    this._newItem = value;
                    this.OnPropertyChanged();
                }
            }
        }

        public ICommand Remove { get; set; }

        public ICommand Add { get; set; }

        public string Status
        {
            get
            {
                return this._status;
            }
            set
            {
                if (this._status != value)
                {
                    this._status = value;
                    this.OnPropertyChanged();
                }
            }
        }

        public string PosterStatus
        {
            get
            {
                return this._status;
            }
            set
            {
                if (this._status != value)
                {
                    this._status = value;
                    this.OnPropertyChanged();
                }
            }
        }

        public string AvatarStatus
        {
            get
            {
                return this._status;
            }
            set
            {
                if (this._status != value)
                {
                    this._status = value;
                    this.OnPropertyChanged();
                }
            }
        }


        public string ImagePath
        {
            get
            {
                return this._imagePath;
            }
            set
            {
                if (this._imagePath != value)
                {
                    this._imagePath = value;
                    this.OnPropertyChanged();
                }
            }
        }



        protected ViewModelCollection()
        {
            this.Status = "We weren't able to load data.";
            this.Remove = new RemoveCommand<T>(this);
            this.Add = new AddCommand<T>(this);
            this.NewItem = null;
            this.ImagePath = "";
        }

        public override async void LoadData()
        {
            this.Status = "Loading...";
            await Task.Delay(1000);
            this.Service.LoadAll();
            this.Status = "Data were loaded.";

            OnPropertyChanged("Items");
        }
    }
}