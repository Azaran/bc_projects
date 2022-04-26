using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using TaskManager.Model.Properties;

namespace IW5Project.Model
{
    public class BaseModel: INotifyPropertyChanged
    {
        public Guid Id { get; private set; }

        protected BaseModel()
        {
            this.Id = Guid.NewGuid();
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
    }
}
