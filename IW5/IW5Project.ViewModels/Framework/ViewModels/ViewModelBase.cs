using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using IW5Project.Model;
using IW5Project.Services;
using IW5Project.Services.Repository;
using IW5Project.ViewModels.Annotations;

namespace IW5Project.ViewModels.Framework.ViewModels
{
    public abstract class ViewModelBase<T> : INotifyPropertyChanged, IDisposable
        where T: BaseModel
    {
        private bool _disposed;
        private string _name;


        public Repository<MoviesDbContext, T> Service { get; protected set; }


        public string Name
        {
            get
            {
                return _name;
            }
            set
            {
                if (value == _name)
                {
                    return;
                }
                _name = value;
                OnPropertyChanged();
            }
        }

        ~ViewModelBase()
        {
            Dispose(false);
        }

        public void Dispose()
        {

            Dispose(true);
            GC.SuppressFinalize(this);

        }
        public abstract void LoadData();
        public void SaveData()
        {
            this.Service.Save();
        }

        private void Dispose(bool disposing)
        {
            if (!this._disposed)
            {
                if (disposing)
                {
                    this.Service.Dispose();
                }
                this._disposed = true;
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = this.PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }
        public virtual void OnProgramShutdownStarted(object sender, EventArgs e)
        {
            this.Service.Save();
            this.Dispose();
        }
    }
}